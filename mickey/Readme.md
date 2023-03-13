## Mickey 2.0 explanation

Kelas Mickey memiliki beberapa atribut, yaitu state yang merepresentasikan state Mickey 2.0, iv yang merepresentasikan vektor inisialisasi, dan ivIndex yang merepresentasikan indeks vektor inisialisasi yang saat ini digunakan. Kelas ini juga memiliki beberapa metode, yaitu konstruktor Mickey() yang digunakan untuk inisialisasi objek Mickey dengan kunci dan IV, metode encrypt() yang digunakan untuk melakukan enkripsi, dan metode decrypt() yang digunakan untuk melakukan dekripsi.

Metode init() pada kelas Mickey digunakan untuk menginisialisasi state dengan menggunakan kunci dan IV. Metode generateKeystreamByte() digunakan untuk menghasilkan sebuah byte dari keystream dengan menggunakan algoritma Mickey 2.0. Metode encrypt() menggunakan generateKeystreamByte() untuk menghasilkan keystream, dan kemudian melakukan operasi XOR pada setiap byte data dengan byte keystream yang dihasilkan untuk menghasilkan byte ciphertext. Metode decrypt() melakukan operasi yang sama dengan encrypt(), sehingga dapat melakukan dekripsi pada ciphertext menjadi plaintext.

Kelas Mickey menggunakan tipe data uint8_t dan uint32_t dari pustaka stdint.h. Ada juga sebuah definisi konstanta MICKEY_IV_SIZE yang digunakan untuk menentukan ukuran IV. Dalam implementasinya, setiap atribut dan metode pada kelas Mickey memiliki hak akses private atau public yang sesuai dengan fungsinya.
