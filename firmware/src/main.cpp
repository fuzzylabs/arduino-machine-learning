#include <Arduino.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
//#include "tensorflow/lite/micro/testing/micro_test.h"

#include "model.h"

#define LED 13

tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = &micro_error_reporter;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  // if (model->version() != TFLITE_SCHEMA_VERSION) {
  //   TF_LITE_REPORT_ERROR(error_reporter,
  //                        "Model provided is schema version %d not equal "
  //                        "to supported version %d.\n",
  //                        model->version(), TFLITE_SCHEMA_VERSION);
  // }
}

int angle = 0.0;

void loop() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }

  angle = (angle + 1) % 360;

  Serial.printf("Angle %d\n", angle);

  /**
   * Model test
   */

  // Instantiate model
  const tflite::Model* model = ::tflite::GetModel(g_model);

  tflite::AllOpsResolver resolver;

  // Allocate memory for tensors
  const int tensor_arena_size = 2 * 1024;
  alignas(16) uint8_t tensor_arena[tensor_arena_size];

  // Instantiate interpreter
  tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, tensor_arena_size, error_reporter);

  // Allocate tensors
  interpreter.AllocateTensors();

  TfLiteTensor* input = interpreter.input(0);
  Serial.printf("Input size %d\n", input->dims->size);
  Serial.printf("Input %d %d\n", input->dims->data[0], input->dims->data[1]);

  // Provide an example input
  input->data.f[0] = angle * 3.14/180;
  Serial.printf("Input became %d %d\n", input->dims->data[0], input->dims->data[1]);

  // Invoke model with input
  TfLiteStatus invoke_status = interpreter.Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed\n");
  }

  Serial.printf("Invoke status: %d\n", invoke_status);

  // Obtain output
  TfLiteTensor* output = interpreter.output(0);

  
  //Serial.printf("Output: size %d; v0 %d; v1 %d\n", output->dims->size, output->dims->data[0], output->dims->data[1]);
  Serial.printf("Output size %d\n", output->dims->size);
  Serial.printf("Output %d %d\n", output->dims->data[0], output->dims->data[1]);
  Serial.printf("\n");

  delay(1000);
}
