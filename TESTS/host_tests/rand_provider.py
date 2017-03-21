from mbed_host_tests import BaseHostTest
import random
import sys

class RandProvider(BaseHostTest):
    """ Simple, basic host test's test runner waiting for serial port
        output from MUT, no supervision over test running in MUT is executed.
    """

    __result = None
    name = "rand_provider"

    def _callback_return_rand(self, key, value, timestamp):
        # You've received {{return_rand;*}}

        # We will send DUT some data back...
        # And now decide about test case result
        if value == 'seed':
            # Message payload/value was 'some_stuff'
            # We can for example return true from test
            seed = random.randint(0, sys.maxint)
            self.send_kv("seed", "%d" % seed)
            self.notify_complete(True)
        else:
            self.send_kv("print_this", "This not what I wanted :(")
            self.notify_complete(False)

    def setup(self):
        # Register callback for message 'return_rand' from DUT
        self.register_callback("return_rand", self._callback_return_rand)

        # Initialize your host test here
        # ...

    def result(self):
        # Define your test result here
        # Or use self.notify_complete(bool) to pass result anytime!
        return self.__result

    def teardown(self):
        # Release resources here after test is completed
        pass