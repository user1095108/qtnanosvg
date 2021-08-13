![screenshot.png](screenshot.png?raw=true)
# qtnanosvg
These are some utilities ported over from the [CairoWidget](https://github.com/user1095108/cairowidget) project. Qt's own SVG rendering is notoriously slow and buggy, it can even crash your program. [NanoSVG](https://github.com/memononen/nanosvg), though also buggy and incomplete, is an easy and reliable alternative. Using vector graphics in your project solves the problem of resolution-independent resources.
# build instructions
    git submodule init
    git submodule update
    qmake
    make
