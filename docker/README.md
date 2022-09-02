## Dockerfile for easier installation (hopefully)

Install docker

In the docker folder:

```
sudo docker build -t moos/latest .
```

Then run ./run_moos.sh to run the image. The script takes in a working directory and renames it "working" in the docker image.
