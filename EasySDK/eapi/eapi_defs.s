

; error codes
EAPI_ERR_RAM            = 1
EAPI_ERR_ROML           = 2
EAPI_ERR_ROMH           = 3
EAPI_ERR_ROML_PROTECTED = 4
EAPI_ERR_ROMH_PROTECTED = 5

; I/O address used to select the bank
EASYFLASH_IO_BANK    = $de00

; I/O address for enabling memory configuration, /GAME and /EXROM states
EASYFLASH_IO_CONTROL = $de02

; Bit for Expansion Port /GAME line (1 = low)
EASYFLASH_IO_BIT_GAME    = $01

; Bit for Expansion Port /EXROM line (1 = low)
EASYFLASH_IO_BIT_EXROM   = $02

; Bit for memory control (1 = enabled)
EASYFLASH_IO_BIT_MEMCTRL = $04

; Bit for status LED (1 = on)
EASYFLASH_IO_BIT_LED     = $80
