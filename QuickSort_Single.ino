#include <Arduino.h>
#include <esp_timer.h>

// ===== CONFIGURAÇÕES AJUSTÁVEIS ===== //
#define ARRAY_SIZE 25200  // Altere aqui para aumentar/diminuir a carga
#define NUM_RUNS   5     // Número de execuções para calcular a média
#define RAND_SEED  12345 // Semente fixa para reprodutibilidade
// ==================================== //

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

  // Pré-aquecimento (execução descartada)
  for (int i = 0; i < ARRAY_SIZE; i++) arr[i] = random(1000);
  quickSort(arr, 0, ARRAY_SIZE - 1);

  // Medição principal (com média de NUM_RUNS execuções)
  uint64_t total_time = 0;
  for (int run = 0; run < NUM_RUNS; run++) {
    randomSeed(RAND_SEED); // Reset da semente para reprodutibilidade
    for (int i = 0; i < ARRAY_SIZE; i++) arr[i] = random(1000);

    uint64_t start = esp_timer_get_time();
    quickSort(arr, 0, ARRAY_SIZE - 1);
    total_time += esp_timer_get_time() - start;
  }

  Serial.printf("QuickSort para %d elementos | Tempo médio: %llu us\n", 
                ARRAY_SIZE, total_time / NUM_RUNS);
}

void loop() {}