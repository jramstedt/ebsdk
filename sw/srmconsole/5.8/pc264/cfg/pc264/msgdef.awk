# Message definitions
BEGIN {
    message_prefix = "msg_"
    message_file = "cp$src:generic_messages.c"

    message_values["msg_success"] = 0
    message_name[0] = "msg_success"
    message_values["msg_failure"] = 1
    message_name[1] = "msg_failure"
    message_values["msg_def"] = 2
    message_name[2] = "msg_def"
    message_values["msg_halt"] = 3
    message_name[3] = "msg_halt"
    message_values["msg_loop"] = 4
    message_name[4] = "msg_loop"
    message_values["msg_error"] = 5
    message_name[5] = "msg_error"
}

/msg_[a-zA-Z0-9_$]/ { collect_messages($0) }
