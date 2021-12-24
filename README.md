# LiquidEqualizerWidget
Modern 15-band equalizer for C++/Qt

Check my other Qt widgets/add-ons out: <https://timschneeberger.me/qt>

## Documentation

#### Setting a single band

```c++
void setBand(int band, float value, bool animate = true);
```

Sets band (0-14) to specified value.

_Optional parameters_:

`animate`: animate changes

#### Setting multiple bands

```c++
void setBands(QVector<float> vector, bool animate = true);
```

Same as `setBand` except it takes a vector of floats is an input and sets them all at once.

#### Getting a single band

```c++
float getBand(int band);
```

Returns value of band (0-14).

#### Getting multiple bands

```c++
QVector<float> getBands();
```

Same as `getBand` except it returns a vector of float containing the values of all bands.

#### Other Properties

##### Grid

```c++
bool getGridVisible() const;
void setGridVisible(bool gridVisible);
```

Sets/gets horizontal background grid visibility.

##### Handles

```c++
bool getAlwaysDrawHandles() const;
void setAlwaysDrawHandles(bool alwaysDrawHandles);
```

Defines whether handles should always be drawn or just on user interaction.

##### Colors

```c++
QColor getAccentColor() const;
void setAccentColor(const QColor &accentColor);
```

Sets/gets accent color.

The background and grid color can be changed by loading a modified `QPalette`.

##### Animation

```c++
int getAnimationDuration() const;
void setAnimationDuration(int animationDuration);
```

Sets/gets the duration of variant animations used in `setBand` or `setBands`.


## Screenshots

Manual use:

![GIF](/screenshots/manual.gif)

Animate changes:

![GIF](/screenshots/animation.gif)

_______

Licensed under GPLv3

