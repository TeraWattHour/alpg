create type search_algolia_result as (document jsonb);

CREATE OR REPLACE FUNCTION search_algolia(index_name text, search_string text, attributes_to_retrieve text, page integer default 0, per_page integer default 30)
    RETURNS SETOF search_algolia_result
    AS 'MODULE_PATHNAME', 'search_algolia'
    LANGUAGE C strict;