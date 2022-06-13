#include <Arduino.h>
#include <stdlib.h>

class kp_dfplayer_mini
{
  public:
    Stream *_stream;
  public:
    kp_dfplayer_mini(Stream &serial);
    void set_volume(uint8_t vol);
    void play_mp3(uint8_t foler_index, uint8_t mp3_index);
    void stop_mp3();
    void mp3_off();
};

kp_dfplayer_mini::kp_dfplayer_mini(Stream &serial)
{
  _stream = &serial;
}

void kp_dfplayer_mini::set_volume(uint8_t vol)
{
  unsigned int check_sum = 0;
  unsigned char *buffer;
  unsigned char code;

  _stream->write(0x7E); // start byte
  code = 0xFF; // ver byte
  _stream->write(code);
  check_sum += code;
  code = 0x06; // len byte
  _stream->write(code);
  check_sum += code;
  code = 0x06; // cmd byte
  _stream->write(code);
  check_sum += code;
  code = 0x00; // feedback byte
  _stream->write(code);
  check_sum += code;
  code = 0x00; // para1 byte
  _stream->write(code);
  check_sum += code;

  _stream->write(vol); // para2 byte
  check_sum += vol;
  check_sum = -check_sum;
  buffer = (unsigned char *)&check_sum;
  _stream->write(buffer[1]); // checksum
  _stream->write(buffer[0]); // checksum
  _stream->write(0xEF); // end byte
}

void kp_dfplayer_mini::play_mp3(uint8_t folder_no, uint8_t file_no)
{
  unsigned int check_sum = 0;
  unsigned char *buffer;
  unsigned char code;

  _stream->write(0x7E); // start byte
  code = 0xFF; // ver byte
  _stream->write(code);
  check_sum += code;
  code = 0x06; // len byte
  _stream->write(code);
  check_sum += code;
  code = 0x0F; // cmd byte
  _stream->write(code);
  check_sum += code;
  code = 0x00; // feedback byte
  _stream->write(code);
  check_sum += code;
  code = folder_no; // para1 byte
  _stream->write(code);
  check_sum += code;

  code = file_no; // para2 byte
  _stream->write(code); // para2 byte
  check_sum += code;
  check_sum = -check_sum;
  buffer = (unsigned char *)&check_sum;
  _stream->write(buffer[1]); // checksum
  _stream->write(buffer[0]); // checksum
  _stream->write(0xEF); // end byte
}

void kp_dfplayer_mini::stop_mp3()
{
  unsigned int check_sum = 0;
  unsigned char *buffer;
  unsigned char code;

  _stream->write(0x7E); // start byte
  code = 0xFF; // ver byte
  _stream->write(code);
  check_sum += code;
  code = 0x06; // len byte
  _stream->write(code);
  check_sum += code;
  code = 0x16; // cmd byte
  _stream->write(code);
  check_sum += code;
  code = 0x00; // feedback byte
  _stream->write(code);
  check_sum += code;
  code = 0x00; // para1 byte
  _stream->write(code);
  check_sum += code;

  code = 0x00; // para2 byte
  _stream->write(code); // para2 byte
  check_sum += code;
  check_sum = -check_sum;
  buffer = (unsigned char *)&check_sum;
  _stream->write(buffer[1]); // checksum
  _stream->write(buffer[0]); // checksum
  _stream->write(0xEF); // end byte

}

void kp_dfplayer_mini::mp3_off()
{
  unsigned int check_sum = 0;
  unsigned char *buffer;
  unsigned char code;

  _stream->write(0x7E); // start byte
  code = 0xFF; // ver byte
  _stream->write(code);
  check_sum += code;
  code = 0x06; // len byte
  _stream->write(code);
  check_sum += code;
  code = 0x0A; // cmd byte
  _stream->write(code);
  check_sum += code;
  code = 0x00; // feedback byte
  _stream->write(code);
  check_sum += code;
  code = 0x00; // para1 byte
  _stream->write(code);
  check_sum += code;

  code = 0x00; // para2 byte
  _stream->write(code); // para2 byte
  check_sum += code;
  check_sum = -check_sum;
  buffer = (unsigned char *)&check_sum;
  _stream->write(buffer[1]); // checksum
  _stream->write(buffer[0]); // checksum
  _stream->write(0xEF); // end byte
}
