BEGIN = -1
QUESTIONS = -2
DINNER = -3
MOUNTAINS = -4
SECRET = -5
msgs = [
    (
        BEGIN,
        [
            ("HEY MUM", "YES"),
            ("HEY MUM", "YES WHAT IS IT"),
            ("HEY MUM", "YES"),
            ("HEY MUM", "YES WHAT IS IT"),
            ("HEY DAD", "YES"),
            ("HEY DAD", "YES SON"),
            ("HEY DAD", "YES"),
            ("HEY DAD", "YES SON"),
            ("HEY DAD", "WHAT IS IT"),
            ("HEY DAD", "YES"),
            ("ARE WE THERE YET", "NOT YET"),
            ("ARE WE THERE YET", "NO"),
            ("ARE WE THERE YET", "JUST A BIT LONGER"),
            ("WHEN WILL WE GET THERE", "I DONT KNOW"),
            ("HOW ABOUT SOME MUSIC", "OK"),
            ("IM BORED", "OK"),
        ],
    ),
    (
        QUESTIONS,
        [
            ("WHERE ARE WE GOING", "TO THE MOUNTAINS"),
            (
                "WHERE ARE WE GOING",
                "TO THE MOUNTAINS",
                "WHERE ARE THE MOUNTAINS",
                "JUST UP AHEAD",
                "OH",
            ),
            (
                "WHERE ARE WE GOING",
                "TO THE MOUNTAINS",
                "WHERE ARE THE MOUNTAINS",
                "JUST UP AHEAD",
                "WHEN WILL WE GET THERE",
                "JUST A BIT LONGER",
            ),
            ("WHERE ARE WE GOING", "TO A SECRET PLACE"),
            (
                "WHERE ARE WE GOING",
                "TO A SECRET PLACE",
                "WHERE IS THE SECRET PLACE",
                "ITS A SECRET",
                "OH",
            ),
            ("ARE WE THERE YET", "NOT YET"),
            ("ARE WE THERE YET", "JUST A BIT LONGER"),
            ("ARE WE THERE YET", "NOT YET"),
            ("ARE WE THERE YET", "JUST A BIT LONGER"),
            ("WHEN WILL WE GET THERE", "I DONT KNOW"),
            ("WHEN WILL WE GET THERE", "IN A JIFFY"),
            ("WHEN WILL WE GET THERE", "JUST A BIT LONGER"),
            ("WHAT IS FOR DINNER", "I DONT KNOW"),
            ("WHAT IS FOR DINNER", "FOOD"),
            (
                "WHAT IS FOR DINNER",
                "WHAT DO YOU WANT",
                "CAN WE HAVE HAMBURGERS",
                "NO YOU HAD THAT LAST TIME",
                "OH",
            ),
            (
                "WHAT IS FOR DINNER",
                "WHAT DO YOU WANT",
                "CAN WE HAVE HAMBURGERS",
                "OK",
                "YES",
            ),
        ],
    ),
]

words = list()


def get_word_id(word):
    global words
    if word not in words:
        words.append(word)
    return words.index(word)


m_msg = list()


def map_msg(msg):
    global m_msg
    if msg not in m_msg:
        m_msg.append(msg)
    return m_msg.index(msg)


def map_conv(conv):
    r = [
        map_msg(tuple([get_word_id(word) for word in msg.split(" ")]))
        if isinstance(msg, str)
        else msg
        for msg in conv
    ]
    if r[-1] > 0:
        r.append(BEGIN)
    return r


convs = []
for msg_set in msgs:
    set_id = msg_set[0]

    s = msg_set[1]
    set_len = len(msg_set[1])
    if set_len == 4:
        s = s * 4

    for conv in msg_set[1]:
        m_conv = map_conv(conv)
        convs.append(m_conv)
        print(m_conv)
    print(set_id, set_len, len(s))
print(m_msg, len(m_msg))
print(words, len(words), max([len(w) for w in words]))

n_words = 2 ** (len(words) - 1).bit_length()
print("n_words", n_words)
with open("words.hex", "w") as f:
    for i in range(n_words):
        if i < len(words):
            enc = words[i].encode("ascii")
        else:
            enc = b""
        enc += b"\0" * (16 - len(enc))
        for c in enc:
            f.write(f"{c:02x} ")
        f.write("\n")

with open("msgs.hex", "w") as f:
    n = 0
    for msg in m_msg:
        for i in range(8):
            if i < len(msg):
                f.write(f"{msg[i]:02x} ")
            else:
                f.write(f"{64:02x} ")
        f.write("\n")
        n += 1
    while n < 32:
        for i in range(8):
            f.write(f"{64:02x} ")
        f.write("\n")
        n += 1

with open("conv.hex", "w") as f:
    for c in convs:
        for i in range(8):
            if i < len(c) and c[i] >= 0:
                f.write(f"{c[i]:02x} ")
            else:
                f.write(f"{31:02x} ")
        f.write("\n")
