#include <Arduino_BMI270_BMM150.h>

#include <TensorFlowLite.h>

#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/kernels/all_ops_resolver.h" //TODO: ersätt
#include "tensorflow/lite/micro/micro_interpreter.h"
//#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "tensorflow/lite/c/common.h"
 //accelerometer functions
//#include "accelerometer_handler.cpp"
#include "model_data.h"

//Compatability
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
int inference_count = 0;

constexpr int TENSOR_ARENA_SIZE = 60 * 1024; //Kan behöva justeras för att matcha minnesbehov bättre
uint8_t tensor_arena[TENSOR_ARENA_SIZE];
} //namespace

void setup(){

 

// if (!IMU.begin()) {
//     Serial.println("Failed to initialize IMU!");
//     while (1);
//   }

  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;


  model = tflite::GetModel(model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  //TODO: Byt ut detta mot MicroOpResolver när vi vet vilka och hur många operationer som används
  static tflite::ops::micro::AllOpsResolver resolver;

  //static tflite::MicroOpResolver<1> resolver; //byt ettan till antal operationer
  //resolver.AddBuiltin(tflite::SOME_OPERATION,tflite::ops::micro::SOME_OPERATION());

  static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, TENSOR_ARENA_SIZE, error_reporter);
  interpreter = &static_interpreter;

  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }
  
  //In-pointer
  input = interpreter->input(0);
  output = interpreter->output(0);

}

void loop(){
  float x,y,z;
  // Läs accelerometer och lägg det i input-tensor
  if (IMU.accelerationAvailable()){
    IMU.readAcceleration(x,y,z); //Måste detta omvandlas till färre dimensioner?
    input->data.f[0] = x;
  }
  //else return;

  // Kör autoencodern
    TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed\n");
  }

  float value = output->data.f[0];
}
