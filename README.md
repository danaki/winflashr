`winflashr`: Blink Rstudio window in Windows taskbar
========================================================

`winflashr` is an R package that contains one function, `winflash()`, with one purpose to make you Rstudio blink from rsession. May be usefull to signal the end of a long running comand.


Installation
----------------

```
library(devtools)
install_github("danaki/winflashr")
```

Requirements
---------------

MS Windows

Details
------------

### Usage

`winflash(count = 1)`

### Arguments

`count` number of times to blink

### Examples

```
winflash(10)
```
