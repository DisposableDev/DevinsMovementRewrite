/* Bhaskara's cosine approximation in SM64's binary-angle units. */
f32 coss_bhaskara(u16 angle) {
    u32 foldedAngle = angle & 0x3FFF;
    u32 flipSign = (angle ^ ((u32) angle << 1)) & 0x8000;

    if (angle & 0x4000) {
        foldedAngle = 0x4000 - foldedAngle;
    }

    const f32 piSquared = 1073741824.0f;
    f32 x = foldedAngle;
    f32 square = x * x;
    f32 fourSquare = square + square + square + square;
    f32 cosine = (piSquared - fourSquare) / (piSquared + square);

    return flipSign ? -cosine : cosine;
}

f32 sins_bhaskara(u16 angle) {
    f32 cosine = coss_bhaskara(angle);
    f32 sine = sqrtf(1.0f - cosine * cosine);

    return angle > 0x8000 ? -sine : sine;
}

/* Paired callers share the division instead of evaluating cosine twice. */
void sincos_bhaskara(u16 angle, f32 *sine, f32 *cosine) {
    f32 c = coss_bhaskara(angle);
    f32 s = sqrtf(1.0f - c * c);

    *sine = angle > 0x8000 ? -s : s;
    *cosine = c;
}
