#ifndef LWIP_DECODER_H
#define LWIP_DECODER_H

#define cimg_display 0
#define cimg_verbosity 0

#include <string>
#include <cstring>
#include <node.h>
#include <node_buffer.h>
#include <nan.h>
#include <v8.h>
extern "C" {
#include "jpeglib.h"
}
#include <png.h>
#include <zlib.h>
#include "CImg.h"

using namespace cimg_library;
using namespace v8;
using namespace node;
using namespace std;

typedef string (* buf_dec_f_t)(char *, size_t, CImg<unsigned char> **);

class DecodeBufferWorker : public NanAsyncWorker {
public:
    DecodeBufferWorker(
        NanCallback * callback,
        char * buffer,
        size_t buffsize,
        buf_dec_f_t decoder
    );
    ~DecodeBufferWorker();
    void Execute ();
    void HandleOKCallback ();
private:
    char * _buffer;
    size_t _buffsize;
    buf_dec_f_t _decoder;
    unsigned char * _pixbuf;
    size_t _width;
    size_t _height;
    int _channels;
    bool _trans; // transparency
};

typedef struct {
    unsigned char * src;
    size_t size;
    size_t read;
} pngReadCbData;

struct lwip_jpeg_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

inline void lwip_jpeg_error_exit (j_common_ptr cinfo) {
    lwip_jpeg_error_mgr * lwip_jpeg_err = (lwip_jpeg_error_mgr *) cinfo->err;
    longjmp(lwip_jpeg_err->setjmp_buffer, 1);
}

/**
 * Utility function to take a CIMG object (**tmp), and convert it to 3 channels
 * (if not 3 channels already)
 * Returns an error string if there was an error. Empty string otherwise.
 */
string to3Channels(CImg<unsigned char> ** img);

string decode_jpeg_buffer(char * buffer, size_t size, CImg<unsigned char> ** img);
string decode_png_buffer(char * buffer, size_t size, CImg<unsigned char> ** img);
void pngReadCB(png_structp png_ptr, png_bytep data, png_size_t length);

NAN_METHOD(decodeJpegBuffer);
NAN_METHOD(decodePngBuffer);
void initAll(Handle<Object> exports);

#endif
