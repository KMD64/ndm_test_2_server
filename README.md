A "calculator" server that uses Generic Netlink protocol for communication.
Requests and responses are string-serialized JSON objects (see examples below).

For client part look into https://github.com/KMD64/ndm_test_2_client

Request example:

{
  "action":"add",
  "argument_1":5,
  "argument_2":7
}

Response example:

{
  "result":12
}

Error example

{
  "error":"Invalid message"
}

The application uses json11 library for parsing and serialization of JSON objects. https://github.com/dropbox/json11

