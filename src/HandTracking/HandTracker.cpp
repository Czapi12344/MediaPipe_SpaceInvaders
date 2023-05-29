#include "HandTracker.h"

#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/image_format.pb.h"

using namespace mediapipe;

HandTracker::HandTracker(sf::Vector2f windowSize)
{
	this->windowSize = windowSize;
	if (init() == EXIT_FAILURE)
	{
		std::cerr << "Failed to initialize hand tracking." << std::endl;
		exit(EXIT_FAILURE);
	}
}

int HandTracker::init()
{
	libmp.reset(LibMP::Create(graphConfigTemplate, inputStream));

	libmp->AddOutputStream(outputStream);
	libmp->Start();

	camera.open(0);
	if (!camera.isOpened())
	{
		std::cerr << "Could not open camera." << std::endl;
		return EXIT_FAILURE;
	}
	conversionInit();

	isTracking = true;
	trackingThread = std::thread([this]
								 { startHandTrackingThread(); });
	return EXIT_SUCCESS;
}

void HandTracker::startHandTrackingThread()
{
	while (isTracking)
	{
		process();
		std::vector<std::array<float, 3>> handLandmarks = parseLandmarksFromPacket();

		cv::Mat frameBGR;
		camera.read(frameBGR);

		std::lock_guard<std::mutex> lock(handMutex);
		frameHandLandmarks.clear();
		for (auto &handLandmark : handLandmarks)
		{
			sf::Vector2f frameHandLandmark;
			frameHandLandmark.x = static_cast<int>(handLandmark[0] * frameBGR.cols);
			frameHandLandmark.y = static_cast<int>(handLandmark[1] * frameBGR.rows);
			frameHandLandmarks.push_back(frameHandLandmark);
		}
	}
}

std::vector<sf::Vector2f> HandTracker::getHandLandmarks()
{
	std::lock_guard<std::mutex> lock(handMutex);
	return frameHandLandmarks;
}

sf::Vector2f HandTracker::getHandLandmark(int point)
{
	std::lock_guard<std::mutex> lock(handMutex);
	return point >= frameHandLandmarks.size() ? sf::Vector2f{-1, -1} : frameHandLandmarks[point];
}

sf::Vector2f HandTracker::getWrist()
{
	return getHandLandmark(0);
}

sf::Vector2f HandTracker::getThumbTip()
{
	return getHandLandmark(4);
}

sf::Vector2f HandTracker::getFingerTip()
{
	return getHandLandmark(8);
}

sf::Vector2f HandTracker::getMiddleFingerTip()
{
	return getHandLandmark(12);
}

sf::Vector2f HandTracker::getRingFingerTip()
{
	return getHandLandmark(16);
}

sf::Vector2f HandTracker::getPinkyTip()
{
	return getHandLandmark(20);
}

void HandTracker::close()
{
	isTracking = false;
	trackingThread.join();
	cv::destroyAllWindows();
}

int HandTracker::process()
{
	cv::Mat frameBGR;
	cv::Mat frameRGB;
	camera.read(frameBGR);
	cv::cvtColor(frameBGR, frameRGB, cv::COLOR_BGR2RGB);

	// Feed RGB frame into MP face mesh graph (image data is COPIED internally by LibMP)
	if (!libmp->Process(frameRGB.data, frameRGB.cols, frameRGB.rows, ImageFormat::SRGB))
	{
		std::cerr << "Process() failed!" << std::endl;
		return EXIT_FAILURE;
	}
	libmp->WaitUntilIdle();
	return EXIT_SUCCESS;
}

std::vector<std::array<float, 3>> HandTracker::parseLandmarksFromPacket()
{
	std::vector<std::array<float, 3>> landmarks;

	// unique_ptr is used for convenience, so that DeletePacket is called automatically
	// Alternatively, it is possible to manage deletion manually:
	// const void* packet = libmp->GetOutputPacket(outputStream);
	// LibMP::DeletePacket(packet);
	std::unique_ptr<const void, decltype(&LibMP::DeletePacket)> landmarkPacket(nullptr, LibMP::DeletePacket);

	if (libmp->GetOutputQueueSize(outputStream) > 0)
	{
		landmarkPacket.reset(libmp->GetOutputPacket(outputStream));
	}
	if (landmarkPacket.get() == nullptr || LibMP::PacketIsEmpty(landmarkPacket.get()))
	{
		return landmarks; // return empty vector if no output packets or packet is invalid
	}

	const void *packetProtobufMessage = LibMP::GetPacketProtoMsgAt(landmarkPacket.get(), 0);
	size_t packetProtobufMessageSize = LibMP::GetProtoMsgByteSize(packetProtobufMessage);

	std::shared_ptr<uint8_t[]> protobufData(new uint8_t[packetProtobufMessageSize]);
	LibMP::WriteProtoMsgData(protobufData.get(), packetProtobufMessage, static_cast<int>(packetProtobufMessageSize));

	NormalizedLandmarkList landmarkList;
	landmarkList.ParseFromArray(protobufData.get(), static_cast<int>(packetProtobufMessageSize));

	for (auto &landmark : landmarkList.landmark())
	{
		landmarks.push_back({landmark.x(), landmark.y(), landmark.z()});
	}

	return landmarks;
}

void HandTracker::conversionInit()
{
	cameraSize.x = camera.get(cv::CAP_PROP_FRAME_WIDTH);
	cameraSize.y = camera.get(cv::CAP_PROP_FRAME_HEIGHT);
	scale.x = windowSize.x / cameraSize.x;
	scale.y = windowSize.y / cameraSize.y;
}

sf::Vector2f HandTracker::convert(sf::Vector2f wristCoord)
{
	if (wristCoord.x == -1 && wristCoord.y == -1)
	{
		return wristCoord;
	}
	sf::Vector2f converted;
	converted.x = (cameraSize.x - wristCoord.x) * scale.x;
	converted.y = wristCoord.y * scale.y;
	return converted;
}

/*
 * If the distance between the wrist and the thumb is greater
 *  than the distance between the wrist and the other fingers
 * then fist is closed
 * otherwise not.
 */
bool HandTracker::isFistClosed()
{
	if (getHandLandmarks().size() == 0)
	{
		return false;
	}
	sf::Vector2f wrist = getWrist();
	std::vector<sf::Vector2f> fingers = {
		getThumbTip(),
		getFingerTip(),
		getMiddleFingerTip(),
		getRingFingerTip(),
		getPinkyTip()};

	std::vector<float> distances;
	for (const auto &finger : fingers)
	{
		float distance = euclideanDistance(wrist, finger);
		distances.push_back(distance);
	}

	auto maxDistance = std::max_element(distances.begin(), distances.end());
	return maxDistance == distances.begin();
}

float HandTracker::euclideanDistance(const sf::Vector2f &point1, const sf::Vector2f &point2)
{
	float distanceX = point2.x - point1.x;
	float distanceY = point2.y - point1.y;
	return std::sqrt(distanceX * distanceX + distanceY * distanceY);
}
