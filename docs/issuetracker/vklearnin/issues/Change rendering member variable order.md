---
issueTracker: "[[vklearnin/dashboard.md|Issues]]"
issueNo: 2
status: open
labels:
  - enhancement
---
```dataviewjs
dv.view("Issue Tracker/IssueTracker/Issue", { obsidian: obsidian });
```

Rendering  object member variables (like `_depth_buffer` or `_resolve_buffer`) should exist, be created, and be destroyed in the order they're actually used in the graphics pipeline.