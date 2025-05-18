#include <Arduino.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define ARRAY_SIZE 30000  // Keep the size reduced for testing
#define NUM_RUNS 5
#define RAND_SEED 12345

int arr[ARRAY_SIZE];
volatile bool taskComplete = false; // Signaling between cores

void quickSort(int *arr, int left, int right) {
  if (left < right) {
    int pivot = arr[right];
    int i = left;
    for (int j = left; j < right; j++) {
      if (arr[j] < pivot) {
        std::swap(arr[i], arr[j]);
        i++;
      }
    }
    std::swap(arr[i], arr[right]);
    quickSort(arr, left, i - 1);
    quickSort(arr, i + 1, right);
  }
}

void taskSecondCore(void *params) {
  int *range = (int *)params;
  quickSort(arr, range[0], range[1]);
  taskComplete = true; // Signal completion
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial connection

  // Initial message
  Serial.println("Starting ESP32 dual-core test");
  Serial.printf("Array size: %d\n", ARRAY_SIZE);
  Serial.printf("Initial free memory: %d bytes\n", esp_get_free_heap_size());

  // Warm-up run (single core)
  randomSeed(RAND_SEED);
  for (int i = 0; i < ARRAY_SIZE; i++) arr[i] = random(1000);
  quickSort(arr, 0, ARRAY_SIZE - 1);

  // Dual-core execution
  uint64_t total_time = 0;
  for (int run = 0; run < NUM_RUNS; run++) {
    // Reinitialize array
    randomSeed(RAND_SEED);
    for (int i = 0; i < ARRAY_SIZE; i++) arr[i] = random(1000);
    taskComplete = false; // Reset flag

    // Divide the workload
    int mid = ARRAY_SIZE / 2;
    int range[2] = {mid, ARRAY_SIZE - 1};

    uint64_t start = esp_timer_get_time();

    // Start task on Core 1
    xTaskCreatePinnedToCore(
      taskSecondCore,
      "QuickSortTask",
      2048,  // Stack size
      range,
      1,     // Priority
      NULL,
      1      // Core 1
    );

    // Run on Core 0
    quickSort(arr, 0, mid - 1);

    // Wait for Core 1 to finish
    while (!taskComplete) {
      delay(1); // Small delay to avoid busy-wait
    }

    total_time += esp_timer_get_time() - start;

    Serial.printf("Run %d completed\n", run + 1);
  }

  Serial.printf("Average dual-core time: %llu us\n", total_time / NUM_RUNS);
  Serial.println("Test completed successfully");
}

void loop() {
  // Nothing needed here
  delay(10000);
}
