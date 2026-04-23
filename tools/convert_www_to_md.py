#!/usr/bin/env python3
"""Convert legacy HPL HTML docs into Markdown files with preserved raw HTML."""

from __future__ import annotations

import html
import re
import shutil
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
SRC_DIR = ROOT / "www"
OUT_DIR = ROOT / "www-md"

HTML_FILES = sorted(SRC_DIR.glob("*.html"))
IMAGE_EXTS = {".jpg", ".jpeg", ".png", ".gif", ".webp", ".svg"}


BODY_RE = re.compile(r"(?is)<body\b[^>]*>(.*)</body>")
TITLE_RE = re.compile(r"(?is)<title\b[^>]*>(.*?)</title>")
LOCAL_HTML_HREF_RE = re.compile(
    r'(?P<prefix>\bhref\s*=\s*)(?P<quote>["\']?)(?P<target>[^"\'\s>]+?\.html)(?P<suffix>#[^"\'\s>]*)?(?P=quote)',
    re.IGNORECASE,
)


def extract_title(text: str, fallback: str) -> str:
    match = TITLE_RE.search(text)
    if not match:
        return fallback
    title = re.sub(r"\s+", " ", match.group(1)).strip()
    return html.unescape(title) or fallback


def extract_body(text: str) -> str:
    match = BODY_RE.search(text)
    if not match:
        raise ValueError("Could not locate <body>...</body> block")
    body = match.group(1).strip()
    return body


def rewrite_html_links(body: str) -> str:
    def repl(match: re.Match[str]) -> str:
        prefix = match.group("prefix")
        quote = match.group("quote") or ""
        target = match.group("target")
        suffix = match.group("suffix") or ""
        if "://" in target or re.match(r"^[a-zA-Z][a-zA-Z0-9+.-]*:", target):
            return match.group(0)
        converted = re.sub(r"\.html$", ".md", target, flags=re.IGNORECASE)
        return f"{prefix}{quote}{converted}{suffix}{quote}"

    return LOCAL_HTML_HREF_RE.sub(repl, body)


def markdown_wrapper(title: str, body: str) -> str:
    lines = [
        f"<!-- Converted from {title} -->",
        "",
        body.strip(),
        "",
    ]
    return "\n".join(lines)


def copy_images() -> None:
    for asset in sorted(SRC_DIR.iterdir()):
        if asset.is_file() and asset.suffix.lower() in IMAGE_EXTS:
            shutil.copy2(asset, OUT_DIR / asset.name)


def convert_all() -> None:
    if OUT_DIR.exists():
        shutil.rmtree(OUT_DIR)
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    for html_file in HTML_FILES:
        raw = html_file.read_text(encoding="utf-8")
        title = extract_title(raw, html_file.stem)
        body = extract_body(raw)
        body = rewrite_html_links(body)
        markdown = markdown_wrapper(title, body)
        out_file = OUT_DIR / f"{html_file.stem}.md"
        out_file.write_text(markdown, encoding="utf-8")

    copy_images()


if __name__ == "__main__":
    convert_all()
