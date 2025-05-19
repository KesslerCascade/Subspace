# Localization Guide

## Setup

To begin localization, make a `data` subfolder in the Subspace installation directory, and copy the `data/lang` folder from the source distribution into it.

Since the contents of the `data` directory override the files in subspace.dat, this allows for working on localization without needing to set up a full development tree and recompile from source.

## Adding a language

To add a language, start with copying the `en-us.json` file to `[yourlang].json`, where [yourlang] is te ISO 639 language code, with an optional country code if there are regional variations.

Then, add an entry to `languages.json` to map the language code (which should be the name of your new file without the .json extension) to the display name.

After restarting Subspace, your new language should appear in the list of languages to choose from.

## Translating

This is as simple as editing the json file to enter the appropriate text for the new language. The English file can be used as a reference for which strings are needed.

If new strings are added, the en-us language is used as a fallback for any missing translations.