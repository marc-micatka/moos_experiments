# moos_experiments

Currently, this repo uses a very simple set of nodes to look at a few
different implementation decisions.

## MOOS interface questions

I created these stubs to test some open questions about how best to implement MOOS nodes:

* Tick-based onNewMail vs. callback-based

* Using individual variables, formatted strings, or protobufs to hold messages
  - I *really* like atomic updates.

* if we go with atomic updates, what's the best method of serializing multiple variables into one string or binary blob?
  - Ease of introspection?

## Setup
Start by reading section 3 of the MOOS overview:
https://oceanai.mit.edu/ivpman/pmwiki/pmwiki.php?n=Helm.MOOSOverview

#### Install MOOS
I cloned MOOS from:
https://oceanai.mit.edu/svn/moos-ivp-aro/releases/moos-ivp-19.8.1
and then followed the install instructions.

If you're not able to clone the package, you might first have to Modify ~/.subversion/servers to add under [global]

http-proxy-host = your.proxy.host
http-proxy-port = your.proxy.port
http-proxy-compression = no


```bash
$ sudo apt install subversion build-essential cmake xterm libfltk1.3-dev freeglut3-dev libpng-dev libjpeg-dev libxft-dev libxinerama-dev libtiff5-dev

$ svn co https://oceanai.mit.edu/svn/moos-ivp-aro/releases/moos-ivp-19.8.1 moos-ivp
cd moos-ivp
./build-moos.sh
./build-ivp.sh

//Add MOOS to your path in your bashrc
echo "PATH=${PATH}:~/moos-ivp/bin" >> ~/.bashrc
source ~/.bashrc
```

If you get cmake errors during building, it might be because you don't have proper build permissions for your root directory. Potentially you can try to just "sudo build" but giving specific permissions should also work. Try:

```bash
sudo chmod -R a+rwx cmake_directory
```

#### Install protobuf

Should be able to install protobuf and boost from apt:

```
sudo apt update
sudo apt install protobuf-compiler libboost-all-dev
```

Might have to add the path to bashrc:

In ~/.bashrc
~~~
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib
~~~

## Compile/Run the Examples

As is standard, compile using:
~~~
mkdir build
cd build
cmake ..
make
make install
~~~

Before running anything, make sure that the path to the binaries is on your PATH:
~~~
export PATH=${PATH}:/path/to/moos_experiments/devel/bin
~~~


### Example #1: "Traditional" way -- using one var per variable in message, using MOOSApp::Run

The nodes are:
* talker_classic -- a node that spits out random x/y/z values at a fixed tick rate
* listener_classic -- a node that listens to the talker's messages and publishes
     an average of the last three.

To launch the moos community:

`pAntler missions/classic_demo.moos`

### Example #2: Protobuf + binary message, using CMOOSApp::Run

As in the previous example, we have two nodes: `talker_protobuf` and `listener_protobuf`

They use messages that are defined in the file: `src/test_messages/test_messages.proto`

To launch the moos community:

`pAntler missions/protobuf_demo.moos`

I intentionally wrote the protobuf nodes with copy-and-paste to make it very easy to compare the two different methods:
~~~
meld src/talker/src/talker_classic.cpp src/talker/src/talker_protobuf.cpp
meld missions/classic_demo.moos missions/protobuf_demo.moos
~~~

### Example #3: Traditional Iterate() vs Queues

In a traditional CMoosApp, the node runs at a fixed rate, checking
messages once a cycle. If you have multiple nodes in a single data
pipeline, this can cause noticeable delays, since each relay requires
waiting for that process to tick.

However, MOOS has an option to have a callback called immediately when
a message on a given variable is received. In my experiments, this had
about 1 ms of delay in a 4-hop chain of nodes running at 1 Hz.
For contrast, it was easy to get multiple seconds of latency in the
same setup when using the traditional message handling. Additionally,
the callback based method is nicer to implement: rather than sorting
through all the waiting messages and filtering them by variable name,
the callbacks are associated with the variable name so you already know
what you're getting.

I implemented 4 different nodes to demonstrate this:

1. `source_node` -- publishes a (string-type) message with a sequence number and timestamp
2. `relay_node` -- uses the traditional OnNewMail() function to directly copy the string from input_variable to output_variable
3. `relay_queue_node` -- uses variable-specific queues to copy the string from input_variable to output_variable.
4. `sink_node` -- compares the message's payload with teh current time (from MOOSTime) and prints out the total latency of the pipeline. This node uses the queue-based callback method in order to avoid adding significant additional latency.

To test the classic style:
~~~
pAntler missions/timing_classic_test.moos
~~~

To test the queue style:
~~~
pAntler missions/timing_queue_test.moos
~~~

To look at the differences in the implementations:
~~~
meld src/timing_tests/src/relay_node.cpp src/timing_tests/src/relay_queue_node.cpp
~~~


### Example #4: Python!
It looks like there are pretty simple python bindings available:

* The original ones (by Paul Newman) used Boost.python: https://github.com/themoos/python-moos

* There are newer ones (by msis) using pybind11: https://github.com/msis/python-moos

* Both required patches to support python3, so I have a fork of msis's that I'm using for now: https://github.com/lauralindzey/python-moos/tree/get_binary_data
  - I think MSIS accepted my PR! Check this.

I don't think that we should consider this for any vehicle code, but it
could be useful for scripting/introspection.

Install:
~~~
git clone https://github.com/lauralindzey/python-moos.git
cd python-moos
git checkout get_binary_data
python3 setup.py build
sudo python3 setup.py install
~~~

Make sure that the install directories are on your PYTHONPATH. I edited my `~/.bashrc` to include:
~~~
export PYTHON_PATH=${PYTHON_PATH}:/usr/local/lib/python3.8/dist-packages
~~~

Since we're building python messages as well, we need to be sure that pip's verison of protobuf is up todate:
~~~
pip3 install --upgrade protobuf
~~~

To test it, I wrote a quick script that, given a MOOS variable name and
the name of the corresonding protobuf, will echo the decoded messages.
This could be very useful since protobuf encodings aren't human readable
and uXMS will display that data as indecipherable binary.

e.g. run:
~~~
term1$ pAntler protobuf_demo.moos
term2$ moos_echoer.py LISTENER_DATA Point3
~~~

## Development Practices

Unaviodably, I ran up against a lot of code-craft questions along the way:

* Which OS are we targeting? Ubuntu 18.04 or 20.04?

* Language choice? Does anybody disagree with the default of "only C++ on the vehicle"?

* Which C++ version? Any objections to C++17? (or 20?)

* Any objections to Boost?

* Standard coding style?
  I default to the Google Style Guide just because it's comprehensive,
  well documented, and supported by linters.

* Repo organization? Lots of individual repos, or a big mono-repo?
  In general, I like monorepos because it's nice to not have to deal
  with versioning issues between repositories. I haven't used git
  submodules before, but I *think* that the organization I've proposed
  makes it easy to turn a subdirectory into its own repo if we need to
  break them apart in the future.

* Code (and build) organization? I'm a cmake neophyte, so I'm hoping that somebody
  else has a better sense of how to do this. (However, I don't think that
  the default appraoch used by MOOS is very good either -- overreliance on
  GLOB and relative paths.) In this sandbox, I've followed this pattern:

~~~
moos_experiments
|- README.md
|- CMakeLists.txt  // Top-level; primarily exists to set project-wide variables and call add_subdirectory
|- devel/     // avoid installing system-wide
  |- bin      // all binaries installed here
  |- include  // public header files installed here
  |- lib
|- build/   // I prefer out-of-source builds
|- missions   // All custom nodes required by the mission should be implemented in src/ and installed to bin/
  |- mission1.moos
  |- mission2.moos
|- src
  |- test_messages
    |- CMakeLists.txt
    |- test_messages.proto
    |- __init__.py
  |- node1
    |- CMakeLists.txt
    |- src/   // all *.cpp files
    |- include/node1/   // all header files
  |- node2
    |- CMakeLists.txt
    |- src/
    | -include/node2
~~~
