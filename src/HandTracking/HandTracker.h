#include <libmp.h>
#include <memory>
#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>

class HandTracker
{
public:
    HandTracker(sf::Vector2f windowSize);
    int init();
    std::vector<sf::Vector2f> getHandLandmarks();
    sf::Vector2f getHandLandmark(int point);
    sf::Vector2f getWrist();
    sf::Vector2f getThumbTip();
    sf::Vector2f getFingerTip();
    sf::Vector2f getMiddleFingerTip();
    sf::Vector2f getRingFingerTip();
    sf::Vector2f getPinkyTip();
    sf::Vector2f convert(sf::Vector2f wristCoord);
    bool isFistClosed();
    void close();

private:
    const char *graphConfigTemplate = R"(
        input_stream: "input_video"
        output_stream: "landmarks"

        # Generates side packet cotaining max number of hands to detect/track.
        node {
            calculator: "ConstantSidePacketCalculator"
            output_side_packet: "PACKET:num_hands"
            node_options: {
                [type.googleapis.com/mediapipe.ConstantSidePacketCalculatorOptions]: {
                    packet { int_value: 1 }
                }
            }
        }

        # Detects/tracks hand landmarks.
        node {
            calculator: "HandLandmarkTrackingCpu"
            input_stream: "IMAGE:input_video"
            input_side_packet: "NUM_HANDS:num_hands"
            output_stream: "LANDMARKS:landmarks"
            output_stream: "HANDEDNESS:handedness"
            output_stream: "PALM_DETECTIONS:multi_palm_detections"
            output_stream: "HAND_ROIS_FROM_LANDMARKS:multi_hand_rects"
            output_stream: "HAND_ROIS_FROM_PALM_DETECTIONS:multi_palm_rects"
        }
    )";
    const char *inputStream = "input_video";
    const char *outputStream = "landmarks";
    std::shared_ptr<mediapipe::LibMP> libmp;
    cv::VideoCapture camera;
    std::vector<sf::Vector2f> frameHandLandmarks;
    std::thread trackingThread;
    std::mutex handMutex;
    bool isTracking;
    sf::Vector2f windowSize;
    sf::Vector2f cameraSize;
    sf::Vector2f scale;

    std::vector<std::array<float, 3>> parseLandmarksFromPacket();
    int process();
    void conversionInit();
    float euclideanDistance(const sf::Vector2f &point1, const sf::Vector2f &point2);
    void startHandTrackingThread();
};