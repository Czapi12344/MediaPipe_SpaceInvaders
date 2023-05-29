// Copyright 2023 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package com.google.mediapipe.tasks.vision.facelandmarker;

import com.google.auto.value.AutoValue;
import com.google.mediapipe.formats.proto.LandmarkProto;
import com.google.mediapipe.formats.proto.ClassificationProto.Classification;
import com.google.mediapipe.formats.proto.ClassificationProto.ClassificationList;
import com.google.mediapipe.tasks.components.containers.Category;
import com.google.mediapipe.tasks.components.containers.NormalizedLandmark;
import com.google.mediapipe.tasks.core.TaskResult;
import com.google.mediapipe.formats.proto.MatrixDataProto.MatrixData;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Optional;

/** Represents the face landmarks detection results generated by {@link FaceLandmarker}. */
@AutoValue
public abstract class FaceLandmarkerResult implements TaskResult {

  /**
   * Creates a {@link FaceLandmarkerResult} instance from the list of landmarks, list of face
   * blendshapes classification, and list of facial transformation matrixes protobuf message.
   *
   * @param multiFaceLandmarksProto a List of {@link NormalizedLandmarkList}
   * @param multiFaceBendshapesProto an Optional List of {@link ClassificationList}
   * @param multiFaceTransformationMatrixesProto an Optional List of {@link MatrixData}
   * @throws IllegalArgumentException if there is error creating {@link FaceLandmarkerResult}
   */
  static FaceLandmarkerResult create(
      List<LandmarkProto.NormalizedLandmarkList> multiFaceLandmarksProto,
      Optional<List<ClassificationList>> multiFaceBendshapesProto,
      Optional<List<MatrixData>> multiFaceTransformationMatrixesProto,
      long timestampMs) {
    List<List<NormalizedLandmark>> multiFaceLandmarks = new ArrayList<>();
    for (LandmarkProto.NormalizedLandmarkList faceLandmarksProto : multiFaceLandmarksProto) {
      List<NormalizedLandmark> faceLandmarks = new ArrayList<>();
      multiFaceLandmarks.add(faceLandmarks);
      for (LandmarkProto.NormalizedLandmark faceLandmarkProto :
          faceLandmarksProto.getLandmarkList()) {
        faceLandmarks.add(
            NormalizedLandmark.create(
                faceLandmarkProto.getX(), faceLandmarkProto.getY(), faceLandmarkProto.getZ()));
      }
    }
    Optional<List<List<Category>>> multiFaceBlendshapes = Optional.empty();
    if (multiFaceBendshapesProto.isPresent()) {
      List<List<Category>> blendshapes = new ArrayList<>();
      for (ClassificationList faceBendshapeProto : multiFaceBendshapesProto.get()) {
        List<Category> blendshape = new ArrayList<>();
        blendshapes.add(blendshape);
        for (Classification classification : faceBendshapeProto.getClassificationList()) {
          blendshape.add(
              Category.create(
                  classification.getScore(),
                  classification.getIndex(),
                  classification.getLabel(),
                  classification.getDisplayName()));
        }
      }
      multiFaceBlendshapes = Optional.of(Collections.unmodifiableList(blendshapes));
    }
    Optional<List<float[]>> multiFaceTransformationMatrixes = Optional.empty();
    if (multiFaceTransformationMatrixesProto.isPresent()) {
      List<float[]> matrixes = new ArrayList<>();
      for (MatrixData matrixProto : multiFaceTransformationMatrixesProto.get()) {
        if (matrixProto.getPackedDataCount() != 16) {
          throw new IllegalArgumentException(
              "MatrixData must contain 4x4 matrix as a size 16 float array, but get size "
                  + matrixProto.getPackedDataCount()
                  + " float array.");
        }
        float[] matrixData = new float[matrixProto.getPackedDataCount()];
        for (int i = 0; i < matrixData.length; i++) {
          matrixData[i] = matrixProto.getPackedData(i);
        }
        matrixes.add(matrixData);
      }
      multiFaceTransformationMatrixes = Optional.of(Collections.unmodifiableList(matrixes));
    }
    return new AutoValue_FaceLandmarkerResult(
        timestampMs,
        Collections.unmodifiableList(multiFaceLandmarks),
        multiFaceBlendshapes,
        multiFaceTransformationMatrixes);
  }

  @Override
  public abstract long timestampMs();

  /** Face landmarks of detected faces. */
  public abstract List<List<NormalizedLandmark>> faceLandmarks();

  /** Optional face blendshapes classifications. */
  public abstract Optional<List<List<Category>>> faceBlendshapes();

  /**
   * Optional facial transformation matrix list from canonical face to the detected face landmarks.
   * The 4x4 facial transformation matrix is represetned as a flat column-major float array.
   */
  public abstract Optional<List<float[]>> facialTransformationMatrixes();
}
