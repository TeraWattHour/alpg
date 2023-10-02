# alpg
**Search Algolia indices from Postgres databases.**

## Requirements
- PostgreSQL, best >=14
- right **pg_config**, extension uses paths provided by this command to install itself, if you have multiple PostgreSQL 
instances installed, make sure to select the right one.
- CMake, best >=3.1
- Make

## Installation

1. Build and install the extension:
```
cd alpg;
sudo make;
```

2. Create the extension in your selected database. Make sure that your user has the right privileges.
```postgresql
CREATE EXTENSION alpg;
```

3. Check whether the extension installed correctly.
```postgresql
\df
```
Should display `search_algolia` as one of the available functions.

## Usage
```postgresql
search_algolia(
    index_name text, 
    search_string text, 
    attributes_to_retrieve text, 
    page integer DEFAULT 0, 
    per_page integer DEFAULT 30
) returns setof (document jsonb)
```
where:
- `index_name` required
- `search_string` required
- `attributes_to_retrieve` comma-separated list of document fields, required
- `page` defaults to 0, must be in the range of (0, n>
- `per_page` defaults to 30, must be in the range of (1,1000>
- `document` jsonb objects representing the received documents, one for each row, json fields can be accessed via the '->' operator