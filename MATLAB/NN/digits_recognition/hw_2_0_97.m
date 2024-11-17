load MNISTData.mat
%X_Test 28*28*10000,D_Test 10*10000;X_Train 28*28*60000,D_Train 10*60000

tic
main(X_Test, D_Test, X_Train, D_Train);
time = toc;
disp(time)

function main(X_Test, D_Test, X_Train, D_Train)
    epoch = 1;
    train_num = 60000;
    test_num = 10000;

    conv_core1 = 2 * rand(5, 5, 6) - 1; %pool1=3*3
    conv_core2 = 2 * rand(3, 3, 5) - 1; %pool2=2*2
    w = rand(10, 270);

    for i = 1:epoch
        [conv_core1, conv_core2, w] = Train(conv_core1, conv_core2, w, X_Train, D_Train, train_num);
    end

    % Test(conv_core1, conv_core2, w1, w2, w3, X_Test, D_Test);
    Test(conv_core1, conv_core2, w, X_Test, D_Test, test_num);
end

function [conv_core1, conv_core2, w] = Train(conv_core1, conv_core2, w, X_Train, D_Train, train_num)
    pace = 0.0121;
    conv1_feature = zeros(24, 24, 6);
    pool1_feature = zeros(8, 8, 6);
    conv2_feature = zeros(6, 6, 6, 5);
    pool2_feature = zeros(3, 3, 6, 5);

    for i = 1:train_num
        op_real = D_Train(:, i); %10*1
        ip_real = X_Train(:, :, i);
        %卷积池化
        for j = 1:6
            conv1_feature(:, :, j) = relu(valid(ip_real, conv_core1(:, :, j))); %24*24*5
            pool1_feature(:, :, j) = pool(conv1_feature(:, :, j), 3); %8*8*5

            for k = 1:5
                conv2_feature(:, :, j, k) = relu(valid(pool1_feature(:, :, j), conv_core2(:, :, k))); %6*6*5*5
                pool2_feature(:, :, j, k) = pool(conv2_feature(:, :, j, k), 2); %3*3*5*5
            end

        end

        %全连接
        ip_full_connect = reshape(pool2_feature, 270, 1, 1, 1); %270*1
        op_final_connect = Softmax(w * ip_full_connect); %10*1
        error_final_connect = op_real - op_final_connect; delta_final = error_final_connect; %10*1
        error_connect = w' * delta_final; %225*1
        w = w + pace * delta_final * ip_full_connect';
        %池化卷积
        error_pool2 = reshape(error_connect, size(pool2_feature));
        error_conv2 = zeros(size(conv2_feature));

        for j = 1:6

            for k = 1:5
                error_conv2(:, :, j, k) = reset_pool(error_pool2(:, :, j, k), 2);
            end

        end

        delta_conv2 = (conv2_feature > 0) .* error_conv2; %6*6*5*5

        error_conv1 = zeros(size(conv1_feature));
        error_pool1 = zeros(size(pool1_feature));

        for j = 1:6

            for k = 1:5
                error_pool1(:, :, j) = error_pool1(:, :, j) + full(delta_conv2(:, :, j, k), conv_core2(:, :, k));
            end

        end

        for j = 1:6
            error_conv1(:, :, j) = reset_pool(error_pool1(:, :, j), 3);
        end

        delta_conv1 = (conv1_feature > 0) .* error_conv1; %24*24*5
        d_core1 = zeros(size(conv_core1)); %9*9*5
        d_core2 = zeros(size(conv_core2)); %5*5*5

        for j = 1:6
            d_core1(:, :, j) = pace * valid(ip_real, delta_conv1(:, :, j));

            for k = 1:5
                d_core2(:, :, k) = d_core2(:, :, k) + pace * valid(pool1_feature(:, :, j), delta_conv2(:, :, j, k));
            end

        end

        conv_core1 = conv_core1 + d_core1;
        conv_core2 = conv_core2 + d_core2;
    end

end

function Test(conv_core1, conv_core2, w, X_Test, D_Test, test_num)
    conv1_feature = zeros(24, 24, 6);
    pool1_feature = zeros(8, 8, 6);
    conv2_feature = zeros(6, 6, 6, 5);
    pool2_feature = zeros(3, 3, 6, 5);
    output = zeros(1, test_num);

    for i = 1:test_num
        %卷积池化
        for j = 1:6
            conv1_feature(:, :, j) = relu(valid(X_Test(:, :, i), conv_core1(:, :, j))); %24*24*5
            pool1_feature(:, :, j) = pool(conv1_feature(:, :, j), 3); %8*8*5

            for k = 1:5
                conv2_feature(:, :, j, k) = relu(valid(pool1_feature(:, :, j), conv_core2(:, :, k))); %6*6*5*5
                pool2_feature(:, :, j, k) = pool(conv2_feature(:, :, j, k), 2); %3*3*5*5
            end

        end

        %全连接
        ip_full_connect = reshape(pool2_feature, 270, 1, 1, 1); %270*1
        op_final_connect = Softmax(w * ip_full_connect); %10*1
        [~, maxindex] = max(op_final_connect);
        output(i) = maxindex;
    end

    [~, real_op] = max(D_Test);
    Accuracy = sum(output == real_op(1:test_num)) / test_num;
    % Accuracy = sum(output(1:100) == real_op(1:100)) / 100;
    disp(Accuracy);
end

function v = valid(test, box)
    v = conv(test, box);
end

function v = full(test, box)
    [t, ~] = size(test);
    [b, ~] = size(box);
    full_test = zeros(t + 2 * b - 2, t + 2 * b - 2);
    core = reshape(flip(reshape(box, [], 1)), size(box));

    for p = 1:t

        for q = 1:t
            full_test(p + b - 1, q + b - 1) = test(p, q);
        end

    end

    v = conv(full_test, core);
end

function con = conv(test, box)
    [t, ~] = size(test);
    [b, ~] = size(box);
    con = zeros(t - b + 1, t - b + 1);

    for i = 1:t - b + 1

        for j = 1:t - b + 1
            con(i, j) = sum(sum(box .* test_part(test, i, j, b)));
        end

    end

end

function per_test = test_part(test, i, j, b)
    per_test = zeros(b, b);

    for p = 1:b

        for q = 1:b
            per_test(p, q) = test(i + p - 1, j + q - 1);
        end

    end

end

function v = pool(feature, scale)
    [f, ~] = size(feature);
    v = zeros(f / scale, f / scale);

    for i = 1:f / scale

        for j = 1:f / scale
            v(i, j) = sum(sum(feature((i - 1) * scale + 1:i * scale, (j - 1) * scale + 1:j * scale))) / (scale * scale);
        end

    end

end

%穿越池化层
function v = reset_pool(Error_pool, scale)
    [E, ~] = size(Error_pool);
    v = zeros(E * scale, E * scale);

    for i = 1:scale

        for j = 1:scale
            v(i:scale:end, j:scale:end) = Error_pool / (scale * scale);
        end

    end

end

function y = relu(x)
    y = max(0, x);
end

function y = Softmax(x)
    y = exp(x) / sum(exp(x));
end
