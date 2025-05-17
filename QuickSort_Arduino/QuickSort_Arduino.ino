#define ARRAY_SIZE 879
#define NUM_RUNS   5
#define RAND_SEED  12345

int arr[ARRAY_SIZE];

void swap(int &a, int &b) {
  int temp = a;
  a = b;
  b = temp;
}

void quickSort(int arr[], int left, int right) {
  if (left < right) {
    int pivot = arr[right], i = left;
    for (int j = left; j < right; j++) {
      if (arr[j] < pivot) {
        swap(arr[i], arr[j]);
        i++;
      }
    }
    swap(arr[i], arr[right]);
    quickSort(arr, left, i - 1);
    quickSort(arr, i + 1, right);
  }
}

unsigned long measureTime() {
  unsigned long start = micros();
  quickSort(arr, 0, ARRAY_SIZE - 1);
  return micros() - start;
}

void setup() {
  Serial.begin(9600);
  randomSeed(RAND_SEED);

  // Pré-aquecimento
  for (int i = 0; i < ARRAY_SIZE; i++) arr[i] = random(1000);
  quickSort(arr, 0, ARRAY_SIZE - 1);

  unsigned long total_time = 0;
  for (int run = 0; run < NUM_RUNS; run++) {
    randomSeed(RAND_SEED);
    for (int i = 0; i < ARRAY_SIZE; i++) arr[i] = random(1000);
    total_time += measureTime();
  }

  Serial.print("QuickSort para ");
  Serial.print(ARRAY_SIZE);
  Serial.print(" elementos | Tempo médio: ");
  Serial.print(total_time / NUM_RUNS);
  Serial.println(" us");
}

void loop() {}
