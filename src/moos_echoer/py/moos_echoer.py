#!/usr/bin/env python3

"""
Proof-of-concept utility script that will echo the MOOS messages on a
given topic, including decoding the protobuf for binary messages.

I'm not 100% convinced that the overhead of maintaining python code
is worth the convenience of this utility when compared to a C++
version with a hard-coded list of message classes.

However, the need to specify both the topic and the type is inherent
to our messaging system -- there's no way to decode from the serialized
message itself what type of protobuf generated it, and MOOS doesn't know
either. One option would be to create a lookup table, perhaps as
parameters in the mission file.

There are still a few rough edges:
* The generated python protobuf files aren't installed elegantly --
  it should be "import test_messages_pb2" (or similar)
* Rather than hardcoding the imported messages, it would be better
  to do that dynamically, by taking the module and message name on the
  command line (e.g. test_messages.Point3, rather than just Point3)
* I'd like a `-n 1` option like `rostopic echo`
* Needs to load MOOSDB's hostname and port from mission file (right now,
  it just defaults to localhost:9000).

"""

import argparse
import pymoos
import sys
import time

# TODO: It would be nice to not have to import them explicitly...
import test_messages_pb2.test_messages_pb2 as msgs

class Echoer(object):
    def __init__(self, topic, proto):
        self.topic = topic
        self.proto = proto
        # TODO: set up MOOS subscription to variable
        # TODO: Set up protobuf decoding

        self.comms = pymoos.comms()
        self.comms.set_on_connect_callback(self.on_connect)
        queue_name = "echo_queue"
        self.comms.add_active_queue(queue_name, self.msg_callback)
        self.comms.add_message_route_to_active_queue(queue_name, self.topic)
        # Args are: server, port, my_name
        self.comms.run('localhost', 9000, 'moostopic_echo')

    def on_connect(self):
        self.comms.register(self.topic, 0.0)
        return True

    def msg_callback(self, msg):
        if msg.is_double():
            print("{} (double): {}".format(self.topic, msg.double()))

        elif msg.is_binary():
            ba = bytearray(msg.binary_data())
            pb_type = getattr(msgs, args.proto)
            proto = pb_type.FromString(ba)
            print("{} (binary): \n{}".format(self.topic, proto))

        elif msg.is_string():
            # NOTE: is_string() is True for both binary and string data
            #    so need to check is_binary first.
            print("{} (str): {}".format(self.topic, msg.string()))

        return True

    def run(self):
        # TODO: add option for killing after N messages, like rostopic?
        while True:
            time.sleep(1)



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("topic", type=str, help="MOOS variable to echo")
    parser.add_argument("proto", type=str, help="Type of protobuf to deserialize into")

    args = parser.parse_args()
    try:
        pb_type = getattr(msgs, args.proto)
    except AttributeError:
        print("Unknown protobuf: {}".format(args.proto))
        sys.exit()

    echoer = Echoer(args.topic, args.proto)
    echoer.run()
