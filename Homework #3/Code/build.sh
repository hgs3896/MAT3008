OPTS=-Wno-incompatible-pointer-types-discards-qualifiers 
CC ${OPTS} \
    main.c \
    nrutil.c \
    gaussj.c \
    ludcmp.c \
    svdcmp.c \
    pythag.c \
    mprove.c \
    lubksb.c \
    -o main