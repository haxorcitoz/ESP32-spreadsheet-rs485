# ESP32-spreadsheet-rs485
* Project ini memanfaatkan ESP32 untuk menarik data dari power meter (dalam project ini menggunakan Schneider PM800, OS version 12.200) via koneksi RS485 kemudian mengirim data ke Google Spreadsheet menggunakan wifi.
* Library RS485 yang digunakan berasal dari https://github.com/angeloc/simplemodbusng
* Tutorial langkah koneksi, pengaturan power meter dan konfigurasi ESP32-RS485 bisa dilihat disini https://github.com/fahroniganteng/Arduino-Schneider-Power-Logic-PM5350
* Untuk alamat register tergantung merk, tipe dan versi OS yang digunakan pada power meter.
* Untuk kode pada AppScript Google Spreadsheet silahkan disesuaikan sesuai kebutuhan sendiri.
