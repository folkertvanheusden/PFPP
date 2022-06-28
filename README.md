PixelFlood Picture Pusher

This program tries to send a jpeg as quick as possible continously to a pixelflood server.

If you wonder why this would be useful, then read about pixelflood or ignore this repository :-)


To build this program, you need to have e.g. the libjpeg62-turbo-dev package installed in your computer.

Then: make


Usage:

    PFPP -f my_jpeg_file.jpg -i ip_address_of_pixelflood_server -p port_number -n number_of_threads

If you would like to use the UDP pixelflood format, add the '-u' switch.


License: public domain


Folkert van Heusden <mail@vanheusden.com>
