/**
 * @brief The 1D Proportional-Integral-Derivative class
 *
 */
struct PID
{
public:
    const float kP;           // The tuned p-value
    const float kI;           // The tuned i-value
    const float kD;           // The tuned d-value
    float setpoint = 0;       // The current setpoint of the PID controller
    float errorSumClamp = -1; // The clamp for the error sum, negative to disable clamping
    float outputClamp = -1;
    int minDeltaTime = 0;     // The minimum value that deltaTime is allowed to approach (but never reach) for derivative calculations
    float period = -1;
    float tolerance = -1;

    /**
     * @brief Construct a new PID object
     *
     * @param p Tuned p-value
     * @param i Tuned i-value
     * @param d Tuned d-value
     */
    PID(float p, float i, float d);

    /**
     * @brief Safely resets the setpoint while adjusting the stored values.
     *
     * @param setpoint New setpoint
     */
    void setSetpoint(float setpoint);

    /**
     * @brief Resets the PID controller's integral and derivative values.
     *
     */
    void reset();

    /**
     * @brief Calculates the output given a feedback value
     *
     * @param feedback Current input value
     * @param deltaTime Milliseconds elapsed since last loop run
     * @return PID output
     */
    float calculate(const float &feedback, const unsigned long &deltaTime);

private:
    float errorSum = 0;       // The total sum of past errors
    float lastError = 0;      // The previous measured error
};