#include "mickey.h"
#include <iostream>
#include <cstring>

// Konversi karakter menjadi byte
void charToByte(const char* input, uint8_t* output, size_t length) {
  for (size_t i = 0; i < length; i++) {
    output[i] = static_cast<uint8_t>(input[i]);
  }
}

// Konversi byte menjadi karakter
void byteToChar(const uint8_t* input, char* output, size_t length) {
  for (size_t i = 0; i < length; i++) {
    output[i] = static_cast<char>(input[i]);
  }
}

int main() {
  // Contoh penggunaan Mickey cipher
  uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  uint8_t iv[] = {0x06, 0x07, 0x08, 0x09, 0x0A};
  char data[] = "Hello World";
  size_t dataLength = std::strlen(data);

  // Buat objek dari kelas Mickey
  Mickey mickey(key, sizeof(key), iv, sizeof(iv));

  // Konversi data dari karakter menjadi byte
  uint8_t* dataBytes = new uint8_t[dataLength];
  charToByte(data, dataBytes, dataLength);

  // Enkripsi data
  mickey.encrypt(dataBytes, dataLength);

  // Cetak data hasil enkripsi dalam bentuk karakter
  char* encryptedData = new char[dataLength + 1];
  byteToChar(dataBytes, encryptedData, dataLength);
  encryptedData[dataLength] = '\0';
  std::cout << "Encrypted data: " << encryptedData << std::endl;

  // Dekripsi data
  mickey.decrypt(dataBytes, dataLength);

  // Konversi data dari byte menjadi karakter
  char* decryptedData = new char[dataLength + 1];
  byteToChar(dataBytes, decryptedData, dataLength);
  decryptedData[dataLength] = '\0';

  // Cetak data hasil dekripsi dalam bentuk karakter
  std::cout << "Decrypted data: " << decryptedData << std::endl;

  // Hapus memori yang dialokasikan secara dinamis
  delete[] dataBytes;
  delete[] encryptedData;
  delete[] decryptedData;

  return 0;
}
