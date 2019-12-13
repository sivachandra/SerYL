# Quickstart Guide

All SerYL definitions are listed in the YAML format in a file with extension
`.ycd` instead of `.yaml`. `YCD` stands for **Y**AML **C**lass **D**efinition.

The top level structure in a `.ycd` file must be a YAML mapping. Anything else
is an error.

## The `__package__` entry

The very first key-value entry of the top level mapping must be a
`__package__` entry. It is listed as follows:

```
__package__: seryl.example1
```

The above entry specifies that the definitions listed in the rest of the
`.ycd` file belong to the package named `seryl.example1`.

## Class definition

A class definition is listed using the keyword `class` followed by the name of
the class within parenthesis. The fields of the class are listed as a nested
mapping as follows:

```
class(MyFirstClass):
  MyIntField: int
  MyStringField: string
```

As can be seen, the fields are listed as name-to-type key-value pairs
representing the field name and type respectively. The above example defines
a class with name `MyFirstClass`, having fields `MyIntField` and
`MyStringField`. The field with name `MyIntField` is of type `int, and the
field with name `MyStringField` is of type `string`.

## Enum definition

An enum type definition is listed using the keyword `enum` followed by the name
of the enum within parenthesis. The values of the enum type are listed as a
nested sequence as follows:

```
enum(MyFirstEnum):
  - Item1
  - Item2
  - Item3(127)
```

The above enum example defines an enum type with name `MyFirstEnum` having
values `Item1`, `Item2` and `Item3`. All enum items have a unique integral
value chosen by the implementation. One can provide their own values by
specifying them with parenthesis after the item name. For example, in the
above example, `Item3` has been given a value of `127`.

## Importing other `.ycd` files

One can import other `.ycd` files using the `__import__` key in the top level
mapping as follows:

```
__import__:
  - path/to/file1.ycd
  - path/to/file2.ycd
```

The value corresponding to the `__import__` key should be a sequence listing
the files to be imported. The above example, the files `path/to/file1.ycd`
and `path/to/file2.ycd` are imported.

## Nested types

Enum and class definitions can be nested with other class definitions.

## Builtin types

Class fields can be of another enum/class type, or of a builtin type. A builtin
type is one of the following:

* `bool` - A boolen value
* `int<N>` - An integer type which is `N` bits wide. `N` can be one of
   {8, 16, 32, 64}.
* `int` - Alias for the `int32` type.
* `double` - A double precision floating point value.
* `string` - A string value.

## List types

A class field can be of a list type. List types are specified in a manner
similar to the C++ template types:

```
list<int> # A list of `int` values
list<MyClass> # A list of objects of a previously defined class type `MyClass`
```

## Oneof fields

One can make use of a special type of class field called the oneof field.
They are similar to the [protocol buffer oneof fields](https://developers.google.com/protocol-buffers/docs/proto3#oneof)
and defined in `.ycd` files using the `__oneof__` keyword as follows:

```
class(MyClass):
  __oneof__(<group name>):
    <Name 1>: <type 1>
    <Name 2>: <type 2>
```
