# Little script to download generate images and show them on screen
# only works if you have xdg-open

scp -r <netid>@student-linux.tudelft.nl:~/path/to/lab2/debug/output output
FIRST_IMG=`ls output | head -1`
xdg-open output/$FIRST_IMG

