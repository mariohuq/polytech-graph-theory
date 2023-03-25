@rem charset=cp866
pandoc ^
    --from=markdown+tex_math_dollars+yaml_metadata_block+raw_attribute+inline_code_attributes+definition_lists+pipe_tables "report.md"^
    --to=docx^
    --output="report.docx"^
    --fail-if-warnings^
    --standalone^
    --number-sections^
    --highlight-style config/vs.theme && "report.docx"