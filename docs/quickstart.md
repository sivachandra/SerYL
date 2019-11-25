# Quickstart Guide

All SerYL definitions are listed in the YAML format in a file with extension
`.ycd` instead of `.yaml`. `YCD` stands for **Y**AML **C**lass **D**efinition.

Top level structure in a `.ycd` file must be a YAML mapping. Any else is an
error.

## The `__package__` entry

The very first key-value entry of the top level mapping must be the
`__package__` entry. It is listed as follows:

```
__package__: seryl.example1
```

The above entry specifies that the definitions listed in the rest of the
`.ycd` file belong to the package named `seryl.example1`.i

## Class definition

A class definition is listed using the word `class` followed by the name of
the class within parenthesis. The fields of the class are listed as a nested
mapping as follows:

```
class(MyFirstClass):
  MyIntField: int
  MyStringField: string
```

As can be seen, the fields are listed as string-name to type-name key-value
pairs.

## Enum definition

An enum type definition is listed using the world `enum` followed by the name of
the class enum with parenthesis. The values of the enum type are listed as a
nested sequence as follows:

```
enum(MyFirstEnum):
  - Item1
  - Item2
  - Item3(127)
```

The above enum example defines an enum type with name `MyFirstEnum` with
values `Item1`, `Item2` and `Item3`. All enum items have a unique integral
value chosen by the implementation. One can provide their own values by
specifying them with parenthesis after the item name. In the above example,
`Item3` with have a value of `127`.

## Importing other `.ycd` files

One can import other `.ycd` files using the `__import__` key in the top level
mapping as follows:

```
__import__:
  - path/to/file1
  - path/to/file2
```

The value corresponding to the `__import__` key should be a sequence listing
the files to be imported.

## Nested types

Enum and class definitions can be nested with other class definitions.

