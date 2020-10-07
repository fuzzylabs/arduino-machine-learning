#include <Arduino.h>
#include <math.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
//#include "tensorflow/lite/micro/testing/micro_test.h"

#include "model.h"

#define LED 13

namespace {
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;

  const int kModelArenaSize = 2468;
  const int kExtraArenaSize = 560 + 16 + 100;
  const int kTensorArenaSize = kModelArenaSize + kExtraArenaSize;
  uint8_t tensor_arena[kTensorArenaSize];

  int angle = 0;
}

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  // Setup tensorflow logging
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Instantiate model and resolver
  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.\n",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }
  static tflite::AllOpsResolver resolver;

  // Instantiate a model interpreter
  static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate tensor memory
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);
}

void loop() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }

  angle = (angle + 1) % 360;
  const float x_val = angle * 3.14 / 180;

  input->data.f[0] = x_val;
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Model invoke failed on x_val: %f\n", x_val);
    return;
  }

  const float y_val = output->data.f[0];
  Serial.printf("Inference for %.3f (%d deg) = %.3f :: True sin(%.3f) = %.3f :: error %.3f\n",
                x_val, angle, y_val, x_val, sin(x_val), fabs(sin(x_val) - y_val));

  delay(1000);
}
