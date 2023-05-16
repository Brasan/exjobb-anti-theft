/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <TensorFlowLite.h>
#include "Arduino_BMI270_BMM150.h"
#include "main_functions.h"

#include "sine_model_data.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include <cstdlib>

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
int datapointsRetrieved = 0;
constexpr int DATAPOINTS_PER_INFERENCE = 420; //Amount of measurements (140) * 3 for x, y and z
constexpr float DIFF_THRESHOLD = 0.13;
float x,y,z;
float max, min;
float originalInput[DATAPOINTS_PER_INFERENCE];

// Create an area of memory to use for input, output, and intermediate arrays.
// Finding the minimum value for your model may require some trial and error.
constexpr int kTensorArenaSize = 100 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

void setup() {
  // Set up logging
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_sine_model_data);

  // Replace with micro_ops_resolver?
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) 
  {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  input = interpreter->input(0);
  output = interpreter->output(0);

  pinMode(LED_BUILTIN, OUTPUT);
  IMU.begin();
  delay(5000);
}

void loop() 
{
  if (IMU.accelerationAvailable()){
    IMU.readAcceleration(x,y,z);
    x = (x + 0.21) / (2.23 + 0.21);
    y = (y + 1.18) / (2.23 + 1.18);
    z = (z + 1.4) / (2.03 + 1.4);
  }
  else
  {
    return;
  }
  // to collect data evenly over approx. 3 seconds before inference

  input->data.f[datapointsRetrieved] = x;
  input->data.f[datapointsRetrieved+1] = y;
  input->data.f[datapointsRetrieved+2] = z;

  originalInput[datapointsRetrieved] = x;
  originalInput[datapointsRetrieved+1] = y;
  originalInput[datapointsRetrieved+2] = z;
  
  datapointsRetrieved += 3;

  delay(21);
  // Run inference, and report any error
  if (datapointsRetrieved >= DATAPOINTS_PER_INFERENCE)
  {
    Serial.println("BEGININ");
    for (int i=0; i < DATAPOINTS_PER_INFERENCE; i++)
    {
      Serial.println(input->data.f[i]);
      delay(5);
    }
    Serial.println("ENDIN");
    datapointsRetrieved = 0;
    TfLiteStatus invoke_status = interpreter->Invoke(); //Fill input tensor, then invoke on every single added reading after that for real-time analysis
      if (invoke_status != kTfLiteOk) 
      {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed!");
        return;
      }
    
    Serial.println("BEGINOUT");
    for (int i=0; i < DATAPOINTS_PER_INFERENCE; i++)
    {
      Serial.println(output->data.f[i]);
      delay(5); //To give serial reader on the other side time to read. Might not be needed depending on how serial buffering works?
    }
    Serial.println("ENDOUT");

    float diffSum {0};
    float avgDiff {0};

    for (int i=0; i < DATAPOINTS_PER_INFERENCE; i++)
    {
      diffSum += std::abs(originalInput[i] - output->data.f[i]);
    }

    avgDiff = diffSum/DATAPOINTS_PER_INFERENCE;
    Serial.print("DIFF: ");
    Serial.println(avgDiff);


    if (avgDiff > DIFF_THRESHOLD)
    {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}

void printTensor(TfLiteTensor* tensor){
  for (int i=0; i < DATAPOINTS_PER_INFERENCE; i++){
    Serial.println(tensor->data.f[i]);
  }
}

// float diff(float* input, TfLiteTensor* output){
//   float diffSum {0};
//   float diff[DATAPOINTS_PER_INFERENCE];

//   for (int i=0; i < DATAPOINTS_PER_INFERENCE; i++){
//     diffSum += std::abs(input[i] - output->data.f[i]);
//   }

//   return diffSum/DATAPOINTS_PER_INFERENCE;
// }


