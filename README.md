# MetaFi: CLI Trading Simulator 📈
Simulator trading pasar keuangan berbasis CLI dengan visualisasi grafik candlestick ASCII dinamis. Mengimplementasikan Hash Table untuk autentikasi dan Linked List untuk manajemen data pasar

![Language](https://img.shields.io/badge/Language-C-blue)
![Data Structure](https://img.shields.io/badge/DS-HashTable%20%26%20LinkedList-orange)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)

**MetaFi** adalah aplikasi simulasi trading pasar keuangan (Forex/Crypto) yang berjalan sepenuhnya di terminal (Command Line Interface). Program ini mensimulasikan pergerakan harga pasar secara *real-time* menggunakan grafik candlestick ASCII dan memungkinkan pengguna untuk melakukan transaksi Buy (Long) atau Sell (Short).

## 🌟 Fitur Utama

### 1. Sistem Autentikasi & Akun
* **Register & Login**: Sistem keamanan menggunakan validasi ketat untuk email dan password.
* **Persistance**: Data user dan saldo tersimpan dalam file eksternal (`users.txt`).
* **Hash Table Implementation**: Pencarian dan penyimpanan data user menggunakan algoritma Hashing (Chaining collision handling) untuk performa yang cepat.

### 2. Simulasi Pasar (Market Engine)
* **Dynamic Candlestick Chart**: Grafik harga divisualisasikan menggunakan karakter ASCII yang berwarna (Hijau untuk naik, Merah untuk turun).
* **Price Movement**: Algoritma random walk yang disesuaikan untuk mensimulasikan volatilitas pasar (Open, High, Low, Close).
* **Navigasi Chart**: User bisa melihat history candle sebelumnya (Previous/Next) menggunakan Doubly Linked List.

### 3. Fitur Trading
* **Order Types**: Mendukung posisi **Long** (Buy) dan **Short** (Sell).
* **Risk Management**: Fitur **Take Profit (TP)** dan **Stop Loss (SL)** otomatis. Posisi akan tertutup otomatis jika harga menyentuh titik TP atau SL.
* **History**: Riwayat trading disimpan dan diurutkan berdasarkan profit tertinggi (Priority Queue).

## 📂 Struktur Data yang Digunakan

Proyek ini mengimplementasikan berbagai struktur data manual tanpa library tambahan:
1.  **Hash Table**: Menyimpan data user (`username`, `password`, `balance`).
2.  **Doubly Linked List**: Menyimpan data Candlestick agar chart bisa digeser ke kiri (history) dan kanan (masa depan).
3.  **Singly Linked List (Priority Queue)**: Menyimpan riwayat trading yang terurut berdasarkan besaran profit/loss.
