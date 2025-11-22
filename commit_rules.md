# Commit Message Convention

This document defines the commit naming rules for the project.

## 1. Commit Format

```
<type>: <short description>

[optional body]

[optional footer]
```

---

## 2. Allowed Commit Types

### **feat**

Add a new feature.

```
feat: add SPLL initialization sequence
```

### **fix**

Fix a bug.

```
fix: correct SOSC valid check condition
```

### **init**

Initial project or module creation.

```
init: create SCG and PCC base drivers
```

### **refactor**

Improve code structure without changing behavior.

```
refactor: clean PCC clock gate setup
```

### **docs**

Documentation or README updates.

```
docs: update clock configuration notes
```

### **test**

Add tests or demo examples.

```
test: add demo for LPUART polling mode
```

### **style**

Formatting or naming consistency changes.

```
style: unify register naming convention
```

### **build**

Build system or linker modifications.

```
build: update linker script for RAM usage
```

### **perf**

Performance optimizations.

```
perf: optimize CAN Rx ISR latency
```

### **chore**

Routine maintenance tasks.

```
chore: update .gitignore
```

---

## 3. Short Description Rules

* Written in English
* Lowercase after `:`
* No ending period
* ≤ 50 characters
* Must clearly describe the purpose

**Good examples:**

* `feat: add GPIO init function for PORTC`
* `fix: correct SPLL multiplier config`
* `refactor: split SCG driver into modules`

**Bad examples:**

* `update stuff`
* `fix bug`
* `changes for project`

---

## 4. Body Rules (Optional)

Use when a commit is complex.

* Describe what changed
* Describe why it changed
* Optionally explain impacts

Example:

```
feat: add FlexCAN basic initialization

Implemented register-level setup:
- enable PCC clock
- configure MCR, CTRL1, RXMGMASK
- enable message buffers
```

---

## 5. Footer Rules (Optional)

Used for issue references or breaking changes.

Example:

```
BREAKING CHANGE: SCG clock config now requires SPLL enabled first
```

---

## 6. Commit Template

```
<type>: <short description>

<body: what and why>

<footer: issues or breaking changes>
```
