<div align="center">

# AntiAFK-RBX Announcements [v.3.2.1+]

</div>

`announcement.json` is fetched from the repository raw URL on startup (if enabled). When `enabled` is `true` and `title` is not empty, the main UI shows this announcement instead of the Discord invite row.

In preview builds (alpha/beta/rc), the `announcement.preview.json` file from the `preview` branch is loaded instead of the `announcement.json` file from the `main` branch.

## Legacy / single-object format (`v.3.2.1+` / `v.4.0.0`)

```json
{
    "enabled": true,
    "notify": false,
    "icon": "\uE76E",
    "iconColor": "#ffffff",
    "title": "Title",
    "description": "Optional details"
}
```

## Targeted announcements array (`v.4.1.0+`)

Starting with `v.4.1.0`, an optional `"announcements"` array is checked first (returning the first matching enabled entry; top-level fields remain as a fallback for older versions):

```json
{
    "announcements": [
        {
            "id": "v4.1.0-enjoy",
            "enabled": true,
            "notify": false,
            "notifyOnce": true,
            "minVersion": "4.1.0",
            "maxVersion": "4.9.9",
            "channels": ["stable"],
            "expiresAt": "2026-12-31",
            "icon": "\uEB51",
            "iconColor": "#ff5c8a",
            "title": "Enjoy AntiAFK-RBX!",
            "description": "You can support the project using the Support button below."
        }
    ]
}
```

Fields:

- `enabled`: shows the announcement when `true` and `title` is not empty.
- `notify`: shows a Windows tray notification on startup when `true`.
- `notifyOnce` (`v.4.1.0+`, default `true`): when `id` is set, stores `SeenAnnouncementId` in registry so the tray notification is shown only once per `id`.
- `id` (`v.4.1.0+`): unique identifier (up to 64 chars) used by `notifyOnce`.
- `minVersion` / `maxVersion` / `exactVersion` (`v.4.1.0+`): optional version filters (e.g. `"4.1.0"`, `"4.0.1-beta2"`, or numeric).
- `channels` (`v.4.1.0+`): optional array filter (`"stable"`, `"beta"`, `"alpha"`, `"rc"`, `"preview"`).
- `expiresAt` / `until` (`v.4.1.0+`): optional `YYYY-MM-DD` expiration date.
- `icon`: Segoe MDL2 icon glyph, for example `\uE76E`.
- `iconColor`: icon color as `#RRGGBB`.
- `title`: shown in the main UI and tray menu (up to 80 chars).
- `description`: shown under the title in the main UI (up to 140 chars). Leave empty to hide it.

The destination URL is intentionally not configurable in this file. It depends on the build channel:

- Stable builds open: `https://agzes.github.io/go/to/antiafk-rbx/notify`
- Preview builds (alpha/beta/rc) open: `https://agzes.github.io/go/to/antiafk-rbx/preview-notify`
