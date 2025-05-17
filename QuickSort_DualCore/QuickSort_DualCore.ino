#include <Arduino.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define ARRAY_SIZE 30000  // Mantemos tamanho reduzido para testes
#define NUM_RUNS 5
#define RAND_SEED 12345

int arr[ARRAY_SIZE];
volatile bool taskComplete = false; // Sinalização entre núcleos

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
  taskComplete = true; // Sinaliza conclusão
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  while(!Serial); // Espera conexão serial
  
  // Verificação inicial
  Serial.println("Iniciando teste dual-core ESP32");
  Serial.printf("Tamanho do array: %d\n", ARRAY_SIZE);
  Serial.printf("Memória livre inicial: %d bytes\n", esp_get_free_heap_size());

  // Pré-aquecimento (single core)
  randomSeed(RAND_SEED);
  for (int i = 0; i < ARRAY_SIZE; i++) arr[i] = random(1000);
  quickSort(arr, 0, ARRAY_SIZE - 1);

  // Execução dual-core
  uint64_t total_time = 0;
  for (int run = 0; run < NUM_RUNS; run++) {
    // Reinicializa array
    randomSeed(RAND_SEED);
    for (int i = 0; i < ARRAY_SIZE; i++) arr[i] = random(1000);
    taskComplete = false; // Reset flag

    // Divide o trabalho
    int mid = ARRAY_SIZE / 2;
    int range[2] = {mid, ARRAY_SIZE - 1};

    uint64_t start = esp_timer_get_time();

    // Dispara task no Core 1
    xTaskCreatePinnedToCore(
      taskSecondCore,
      "QuickSortTask",
      2048,  // Stack size
      range,
      1,     // Prioridade
      NULL,
      1      // Core 1
    );

    // Executa no Core 0
    quickSort(arr, 0, mid - 1);

    // Espera o Core 1 terminar
    while(!taskComplete) {
      delay(1); // Pequeno delay para evitar busy-wait
    }

    total_time += esp_timer_get_time() - start;
    
    Serial.printf("Execução %d concluída\n", run+1);
  }

  Serial.printf("Tempo médio dual-core: %llu us\n", total_time / NUM_RUNS);
  Serial.println("Teste finalizado com sucesso");
}

void loop() {
  // Nada necessário aqui
  delay(10000);
}