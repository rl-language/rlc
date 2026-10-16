import bounded_arg
import math.numeric


cls Vector2Df:
    Float x
    Float y

    fun add(Vector2Df other) -> Vector2Df:
        let to_return : Vector2Df
        to_return.x = self.x + other.x
        to_return.y = self.y + other.y
        return to_return

    fun mul(Float other) -> Vector2Df:
        let to_return : Vector2Df
        to_return.x = float(self.x) * other
        to_return.y = float(self.y) * other
        return to_return

    fun div(Float other) -> Vector2Df:
        let to_return : Vector2Df
        to_return.x = float(float(self.x) / other)
        to_return.y = float(float(self.y) / other)
        return to_return

    fun sub(Vector2Df other) -> Vector2Df:
        let to_return : Vector2Df
        to_return.x = self.x - other.x
        to_return.y = self.y - other.y
        return to_return

    fun distance(Vector2Df other) -> Float:
        let x_distance = self.x - other.x
        let y_distance = self.y - other.y
        return sqrt((x_distance * x_distance) + (y_distance * y_distance))

    fun length() -> Float:
        return sqrt(float(self.x * self.x) + float(self.y* self.y))

    fun normalized() -> Vector2Df:
        return self / self.length()

    fun as_int_vector() -> Vector2D:
        let to_return : Vector2D
        to_return.x = int(self.x) 
        to_return.y = int(self.y)
        return to_return

cls Vector2D:
    Int x
    Int y

    fun add(Vector2D other) -> Vector2D:
        let to_return : Vector2D
        to_return.x = self.x + other.x
        to_return.y = self.y + other.y
        return to_return

    fun mul(Float other) -> Vector2D:
        let to_return : Vector2D
        to_return.x = int(float(self.x) * other)
        to_return.y = int(float(self.y) * other)
        return to_return

    fun sub(Vector2D other) -> Vector2D:
        let to_return : Vector2D
        to_return.x = self.x - other.x
        to_return.y = self.y - other.y
        return to_return

    fun distance(Vector2D other) -> Float:
        let x_distance = float(self.x - other.x)
        let y_distance = float(self.y - other.y)
        return sqrt((x_distance * x_distance) + (y_distance * y_distance))

    fun length() -> Float:
        return sqrt(float(self.x * self.x) + float(self.y* self.y))

    fun as_float_vector() -> Vector2Df:
        let to_return : Vector2Df
        to_return.x = float(self.x) 
        to_return.y = float(self.y)
        return to_return

const BOARD_WIDTH = 44
const BOARD_HEIGHT = 30
using CoordinateX = LinearlyDistributedInt<0, BOARD_WIDTH>
using CoordinateY = LinearlyDistributedInt<0, BOARD_HEIGHT>

cls BoardPosition:
    CoordinateX x
    CoordinateY y
    
    fun as_vector() -> Vector2D:
        let vector : Vector2D
        vector.x = self.x.value
        vector.y = self.y.value
        return vector


    fun add(Vector2D other) -> BoardPosition:
        let to_return : BoardPosition 
        to_return.x = self.x + other.x
        to_return.y = self.y + other.y
        return to_return

    fun sub(Vector2D other) -> BoardPosition:
        let to_return : BoardPosition 
        to_return.x = self.x - other.x
        to_return.y = self.y - other.y
        return to_return

fun make_board_position(Int x, Int y) -> BoardPosition:
    let b : BoardPosition
    b.x = x
    b.y = y
    return b

fun make_board_position(CoordinateX x, CoordinateY y) -> BoardPosition:
    let b : BoardPosition
    b.x = x
    b.y = y
    return b

