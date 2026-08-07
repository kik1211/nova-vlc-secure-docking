## PR Description
<!-- Describe the change this PR makes and why. -->

## Related Issue
<!-- Closes #issue_number (if applicable) -->

## Type of Change
- [ ] `feat` — New feature
- [ ] `fix` — Bug fix
- [ ] `docs` — Documentation only
- [ ] `refactor` — Code restructuring (no behavior change)
- [ ] `test` — Adding or fixing tests/validation
- [ ] `chore` — Build config, CI, gitignore

## Subsystem Affected
- [ ] Subsystem 1 — Secure Access
- [ ] Subsystem 2 — Docking
- [ ] Both
- [ ] Infrastructure / Docs only

---

## Code Quality Checklist

- [ ] All new public functions have Doxygen `@brief`, `@param`, `@return` documentation
- [ ] Zero `delay()` calls in HAL or DSP modules
- [ ] Zero dynamic memory allocation (`new`, `malloc`, `free`) in application code
- [ ] All functions ≤ 40 lines
- [ ] Nesting depth ≤ 3 in all functions
- [ ] No magic numbers outside `config.h`
- [ ] Include order follows Implementation Specification §1.10
- [ ] No Arduino `String` type used

## Testing Checklist

- [ ] Manual hardware validation performed (or synthetic test for pure-logic modules)
- [ ] Results documented in `validation/results/` and committed in this PR
- [ ] No regressions in previously validated modules confirmed

## Documentation Checklist

- [ ] `CHANGELOG.md` updated (if significant change)
- [ ] `config.h` constants have `///` Doxygen comments
- [ ] Module-level `@file` Doxygen block present in modified headers

## Design Freeze Compliance

- [ ] No violation of Design Freeze architectural decisions
- [ ] If a design decision changes, an ADR has been written and linked here: ADR-XXX

## Security Checklist

- [ ] No NFC UIDs, credentials, or secrets committed to git history
- [ ] No personally identifiable information in UART log strings
