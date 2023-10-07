# alpg
**Search Algolia indices from Postgres databases.**

## Requirements
- PostgreSQL
- available **pg_config**, this extension uses PGXS
- cJSON 
- cURL
- Make

## Installation

1. Build and install the extension:
```
cd alpg;
make;
```

2. Create the extension in your selected database. Make sure that your user has the right privileges.
```postgresql
CREATE EXTENSION alpg;
```

3. Check whether the extension has been installed correctly.
```postgresql
\dx alpg
```
Should return `alpg` as one of available extensions.

## Usage
Before running extension's functions you need to set up environment variables `alpg.api_key` and `alpg.app_id`. 
Remember to set those values for every session you use since SET is only persistent 
to session or transaction. If even one of those values is undefined, the function will throw.

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