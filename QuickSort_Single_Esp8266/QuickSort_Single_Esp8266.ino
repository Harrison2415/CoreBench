#include <Arduino.h>

// Replaced esp_timer_get_time() with micros() for ESP8266 compatibility
// esp_timer.h is exclusive to ESP32 and causes a compilation error on ESP8266
// micros() returns the time in microseconds since boot (accurate enough for this purpose)

// ===== CONFIGURABLE SETTINGS ===== //
#define ARRAY_SIZE 13250   // Change this to increase/decrease the workload
#define NUM_RUNS   5       // Number of runs to calculate the average
#define RAND_SEED  12345   // Fixed seed for reproducibility
// ================================= //

int arr[ARRAY_SIZE];

void quickSort(int arr[], int left, int right) {
  if (left < right) {
    int pivot = arr[right], i = left;
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

void setup() {
  Serial.begin(115200);
  randomSeed(RAND_SEED);

  // Warm-up run (excluded from measurement)
  for (int i = 0; i < ARRAY_SIZE; i++) arr[i] = random(1000);
  quickSort(arr, 0, ARRAY_SIZE - 1);

  // Main measurement (average of NUM_RUNS executions)
  uint64_t total_time = 0;
  for (int run = 0; run < NUM_RUNS; run++) {
    randomSeed(RAND_SEED); // Reset seed for reproducibility
    for (int i = 0; i < ARRAY_SIZE; i++) arr[i] = random(1000);

    uint32_t start = micros();
    quickSort(arr, 0, ARRAY_SIZE - 1);
    total_time += micros() - start;
  }

  Serial.printf("QuickSort for %d elements | Average time: %llu us\n", 
                ARRAY_SIZE, total_time / NUM_RUNS);
}

void loop() {}
