OPTS=-Wno-incompatible-pointer-types-discards-qualifiers
CC ${OPTS} \
    main.c \
    nrutil.c \
    ludcmp.c \
    lubksb.c \
    -o main