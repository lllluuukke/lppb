#/bin/bash

# Usage: ./resize 0/1 major_img minor_img1 minor_img2 major_img_new minor_img1_new minor_img2_new
if [ $1 -eq 0 ] ; then
mogrify -resize 900x600 $2
mogrify -resize 450x300 $3
mogrify -resize 450x300 $4
else
convert -resize 900x600 $2 $5
convert -resize 450x300 $3 $6
convert -resize 450x300 $4 $7
fi

exit 0

