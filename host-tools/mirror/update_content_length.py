#!/usr/bin/env python

import sqlite3, sys, optparse

def parse():
    parser = optparse.OptionParser(version = "Update the HTTP headers",
                                   usage = "%prog [options] URL")
    parser.add_option("-d", "--db", help = "The crawl database to be used",
                      action = "store", dest = "db_name", default="crawl_db.db")
    return parser.parse_args(sys.argv)

(opts, _) = parse()
connection = sqlite3.connect(opts.db_name)


def to_num(length):
    if not length:
        return 0
    return int(length)

length = len("content-length: ")
cur = connection.execute("SELECT url, header, LENGTH(data) from responses")
for row in cur:
    old_header = str(row[1])
    start = old_header.find("content-length: ")
    end = old_header.find("\r", start)

    old_length = old_header[start + length:end]
    new_length = to_num(row[2])

    if int(old_length) == new_length:
        continue

    new_entry = "content-length: %d" % new_length
    new_header = old_header.replace(old_header[start:end], new_entry, 1)
    connection.execute("UPDATE responses SET header = ? WHERE url like ?", [new_header, row[0]])
connection.commit()
