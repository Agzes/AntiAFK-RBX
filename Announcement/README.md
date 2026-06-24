<div align="center">

# AntiAFK-RBX Announcements [v.3.2.1+]

</div>

`announcement.json` is fetched from the repository raw URL on startup (if on in advanced tab). When `enabled` is `true` and `title` is not empty, the main UI shows this announcement instead of the Discord invite row.

In preview builds (alpha/beta/rc), the `announcement.preview.json` file from the `preview` branch is loaded instead of the `announcement.json` file from the `main` branch.

Supported fields:

```json
{
    "enabled": true,
    "notify": false,
    "icon": "\uE76E",
    "iconColor": "#ffffff",
    "title": "Tittle",
    "description": "Optional details"
}
```

Fields:

- `enabled`: shows the announcement when `true` and `title` is not empty.
- `notify`: shows a Windows tray notification on startup when `true`.
- `icon`: Segoe MDL2 icon glyph, for example `\uE76E`.
- `iconColor`: icon color as `#RRGGBB`.
- `title`: shown in the main UI and tray menu.
- `description`: shown under the title in the main UI. Leave empty to hide it.

The destination URL is intentionally not configurable in this file. It depends on the build channel:

- Stable builds open: `https://agzes.github.io/go/to/antiafk-rbx/notify`
- Preview builds (alpha/beta/rc) open: `https://agzes.github.io/go/to/antiafk-rbx/preview-notify`
