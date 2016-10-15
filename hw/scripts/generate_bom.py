#!/usr/bin/env python3

"""
TODO(halfr): Make checks on partsdb.csv (duplicate {device,value}, duplicate
url, etc.)
"""

import argparse
import collections
import csv
import logging
import sys

BASE_FIELDS = ['refdes', 'device', 'value', 'footprint', 'qty']
ADDITIONAL_FIELDS = ['comment', 'distributor']
ADDITIONAL_FIELDS += [v + '-' + w
        for w in ['buy-url', 'price']
        for v in ['digikey', 'farnell', 'itead']]

def build_partsdb(partsdbcsv):
    """
    Build parts dictonary from csv parts db.

    Usage: partsdb[device][value]
    """
    partsdb = collections.defaultdict(lambda: collections.defaultdict(dict))

    for part in partsdbcsv:
        part['_unused'] = True
        partsdb[part['device']][part['value']] = part


    return partsdb

def process_bom(partslist, partsdb, fields, distributor):
    """
    Generates full BOM (incl. part buying URL and price) from the list of
    parts and a database of parts.
    """
    base_bom_item = [(k, '') for k in fields]
    bom = []

    for bom_item in partslist:
        part = partsdb[bom_item['device']][bom_item['value']]

        # Sanity checks
        if not part:
            logging.warning("Missing: device=%s, value=%s not in parts db",
                            bom_item['device'], bom_item['value'])
            continue

        if part['unused'] == 'yes':
            logging.warning("False unused: device=%s, value=%s is specified unused in parts db but present in parts list",
                            bom_item['device'], bom_item['value'])
        if part['unused'] == 'forced':
            logging.warning("False force: device=%s, value=%s is specified force in parts db but present in parts list",
                            bom_item['device'], bom_item['value'])
        if part['footprint'] and part['footprint'] != bom_item['footprint']:
            logging.warning("Footprint: device=%s, value=%s does not have the same footprint in the parts db (%s) as in the part list (%s)",
                            bom_item['device'], bom_item['value'],
                            part['footprint'], bom_item['footprint'])

        part['_unused'] = False

        if distributor and part['distributor'] != distributor:
            continue

        # Select fields to be in the final bom
        part_add_fields = [(k, v)
                           for k, v in part.items()
                           if k in fields and v]
        bom_fields = [(k, v)
                      for k, v in bom_item.items()
                      if k in fields]

        bom_item = dict(base_bom_item + bom_fields + part_add_fields)
        bom.append(bom_item)

    return bom

def sanity_checks(partsdb):
    for unused_used in [p
                        for p in partsdb
                        if p['unused'] == '' and p['_unused']]:
        logging.warning("Unused: device=%s, value=%s is unused but not marked as such in parts db",
                        unused_used['device'], unused_used['value'])

def write_forced_parts(bom, partsdb, fields, distributor):
    base = dict.fromkeys(fields, '')

    for forced_part in [p
                        for p in partsdb
                        if p['unused'] == 'force']:
        if distributor and forced_part['distributor'] != distributor:
            continue

        forced_fields = {k: v
                         for k, v in forced_part.items()
                         if k in fields}
        forced = base.copy()
        forced.update(forced_fields)
        bom.writerow(forced)

def arglist(s):
    try:
        l = s.split(',')
        return l
    except:
        raise argparse.ArgumentTypeError("Not a comma separated list")

def parse_args():
    parser = argparse.ArgumentParser(
        description='Merge component list and part data')
    parser.add_argument('partslist', type=argparse.FileType('r'),
                        help='List of parts in the project (must generated using gnetlist)')
    parser.add_argument('partsdb', type=argparse.FileType('r'),
                        help='Database of parts')
    parser.add_argument('--fields', type=arglist,
                        help="Comma separated list of fields to output",
                        default=BASE_FIELDS + ADDITIONAL_FIELDS)
    parser.add_argument('--distributor', default=None,
                        help="Output BOM for a specific distributor")

    args = parser.parse_args()
    return args

def main():
    args = parse_args()

    partslist_csv = csv.DictReader(args.partslist, delimiter=':')
    partslist = list(partslist_csv)
    partsdb_csv = csv.DictReader(args.partsdb)
    partsdb = list(partsdb_csv)

    bom_headers = args.fields
    bom = csv.DictWriter(sys.stdout, fieldnames=bom_headers)

    partsdb_dict = build_partsdb(partsdb)
    bom_data = process_bom(partslist, partsdb_dict, args.fields,
                           args.distributor)

    bom.writeheader()
    bom.writerows(bom_data)
    write_forced_parts(bom, partsdb, args.fields, args.distributor)

    sanity_checks(partsdb)

if __name__ == '__main__':
    main()
