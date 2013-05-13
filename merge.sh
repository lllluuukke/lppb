#/bin/bash

# Usage: $ ./merge.sh major_img minor_img1 minor_img2 output_img
convert -page +0+0 ../../template.jpg \
        -page +64+96 $1 \
        -page +34+796 $2 \
        -page +540+796 $3 \
        -layers merge $4

exit 0

