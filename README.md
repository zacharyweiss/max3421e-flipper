# MAX3421E <> F0

WIP example app using custom `furi` glue for `tinyusb`, to allow the Flipper Zero to use the MAX3421E as a USB host controller.

Doing all testing with https://www.adafruit.com/product/5858 currently. Previously was trying with https://www.amazon.com/dp/B01EWW9R1E, 2 of 2 chips bought from Amazon wouldn't properly respond over SPI — recommend avoiding this product and/or seller.

F0 pins as follows:
- (Default external SPI) MISO = PA6
- (Default external SPI) MOSI = PA7
- (Default external SPI) CLK = PB3
- (Default external SPI) CS = PA4
- INT = PB2

TODO:
- Cleanup worker, setup proper callbacks w/ stream buffer & textbox
- Figure out why MAX never asserts `OSCOKIRQ` post-issuing `USBCTL CHIPRES`
  - Others have [reported this as a chip misdesign](https://electronics.stackexchange.com/questions/372236/bugs-in-maxim-chip-max3421e-used-as-usb-host)
  [ ] test replacing wait for `OSCOKIRQ` w/ simple delay
  [ ] PR `tinyusb`?