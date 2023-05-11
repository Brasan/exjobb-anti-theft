#include <Arduino_BMI270_BMM150.h>

#include <TensorFlowLite.h>

#include "main_functions.h"

#include "model_data.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/kernels/all_ops_resolver.h" //TODO: ersätt
#include "tensorflow/lite/micro/micro_interpreter.h"
//#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "tensorflow/lite/c/common.h"
 //accelerometer functions
//#include "accelerometer_handler.cpp"


//Compatability
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
//TfLiteTensor* output = nullptr;
int inference_count = 0;

constexpr int TENSOR_ARENA_SIZE = 100 * 1024; //Kan behöva justeras för att matcha minnesbehov bättre
uint8_t tensor_arena[TENSOR_ARENA_SIZE];
} //namespace

void setup(){
Serial.begin(9600);
Serial.println("Hejsan2");
 

if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
Serial.println("hej");
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
  int input_length = input->bytes / sizeof(float);
  Serial.println(input_length);
  //output = interpreter->output(0);

// if ((input->dims->size != 4) || (input->dims->data[0] != 1) ||
//       (input->dims->data[1] != 128) ||
//       (input->dims->data[2] != kChannelNumber) ||
//       (input->type != kTfLiteFloat32)) {
//     TF_LITE_REPORT_ERROR(error_reporter,
//                          "Bad input tensor parameters in model");
//     return;
//   }
}

void loop(){
  //TF_LITE_REPORT_ERROR(error_reporter, input->type);TF_LITE_REPORT_ERROR(error_reporter, input->type);
  TF_LITE_REPORT_ERROR(error_reporter, "Test1");
  TF_LITE_REPORT_ERROR(error_reporter, (const char*)(input->dims->data[1]));
  TF_LITE_REPORT_ERROR(error_reporter, (const char*)(input->dims->data[2]));
  TF_LITE_REPORT_ERROR(error_reporter, "Test2");
  return;
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

  //float value = output->data.f[0];
}
